;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
LedInit:	bis.b	#40h, P1DIR
			bis.b	#056h, P1SEL
			bis.b	#016h, P1SEL2

USART_Cfg:	bis.b	#UCSWRST, &UCA0CTL1		 ; adjust spi mode, enable SW reset
			mov.b	#UCCKPL+UCMSB+UCSYNC, &UCA0CTL0
			; clock mode 3, MSB first, 8-bit SPI slave, 3 pin mode
			bic.b	#UCSWRST, &UCA0CTL1		 ; clear reset, resume operation
			bis.b	#UCA0RXIE+UCA0TXIE, &IE2 ; enable USCI0 Rx and Tx interrupts

PWM_Setup:	clr.w	TACCR1					 ; adjust the PWM
			mov.w	#03E7h, &TACCR0
			mov.w	#OUTMOD_7, &TACCTL1
			mov.w	#TASSEL_2+MC_1+ID_3, &TACTL

			clr.w	R7						; register used for counting received byte

Interrupts:	bis.w	#GIE, SR

MainLoop:	clr.w	R7
			bis.w	#LPM0, SR
			jmp		MainLoop

;----------------------------------------------
USCIAB0TX_ISR 		; SPI slave transmit ISR
;----------------------------------------------
			mov.b	#0h, &UCA0TXBUF			; send null characters
			bic.b	#UCA0TXIE, &IE2			; disable TX interrupt
			reti

;----------------------------------------------
USCIAB0RX_ISR 		; SPI slave receive ISR
;----------------------------------------------
			bit.w	#1h, R7						; check if R7 is odd
			jne		OddByte					; if it is, jmp to OddByte
			mov.b	UCA0RXBUF, R8			; copy received data (low byte) to LSB of R8
			jmp		IncByteNum

OddByte:	mov.b	UCA0RXBUF, R9			; move received data (low byte) to R9
			swpb	R8
			and.w	#0F00h, R8				; AND R8 with 0x0F00 for the high byte
											; numbers are located in these four bits
			bis.w	R8, R9					; OR R9 and R9
			mov.w	R9, &TACCR1				; move the received two-byte data to TACCR1
			bic.w	#LPM0, 0(SP)			; exit from LPM0

IncByteNum:	inc.w	R7						; icrease byte number by one
			bis.b	#UCA0TXIE, &IE2			; enable TX interrupt
			reti
                                            
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
			.sect USCIAB0TX_VECTOR
			.short USCIAB0TX_ISR
			.sect USCIAB0RX_VECTOR
			.short USCIAB0RX_ISR
			.end
