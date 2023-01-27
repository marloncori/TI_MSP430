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
LedInit:	bis.b	#04h, P1DIR
			bis.b	#0C4h, P1SEL
			bis.b	#0C0h, P1SEL2

USART_Cfg:	bis.b	#UCSWRST, &UCB0CTL1		 ; adjust i2c mode, enable SW reset
			mov.b	#UCMODE_3+UCSYNC, &UCB0CTL0
			; clock mode 3, MSB first, 8-bit I2C slave, synchronous mode
			mov.w	#48h, &UCB0I2COA
			bic.b	#UCSWRST, &UCB0CTL1		 ; clear reset, resume operation
			bis.b	#UCSTPIE+UCSTTIE, &UCB0I2CIE ; enable USCI0 Rx and Tx interrupts
			bis.b	#UCB0RXIE, IE2

PWM_Setup:	clr.w	TACCR1					 ; adjust the PWM
			mov.w	#03E7h, &TACCR0
			mov.w	#OUTMOD_7, &TACCTL1
			mov.w	#TASSEL_2+MC_1+ID_3, &TACTL

Interrupts:	bis.w	#GIE, SR

MainLoop:	clr.w	R7
			bis.w	#LPM0, SR
			jmp		MainLoop

;----------------------------------------------
USCIAB0TX_ISR 		; SPI slave receive ISR
;----------------------------------------------
			bit.w	#1h, R7						; check if R7 is odd
			jne		OddByte					; if it is, jmp to OddByte
			mov.b	UCB0RXBUF, R8			; copy received data (low byte) to LSB of R8
			jmp		IncByteNum

OddByte:	cmp.w	#1h, R7
			jne 	IncByteNum
			mov.b	UCB0RXBUF, R9
			swpb	R8
			and.w	#0F00h, R8				; AND R8 with 0x0F00 for the high byte
											; numbers are located in these four bits
			bis.w	R8, R9					; OR R9 and R9
			mov.w	R9, &TACCR1				; move the received two-byte data to TACCR1

IncByteNum:	inc.w	R7						; icrease byte number by one
			reti
                                            
;----------------------------------------------
USCIAB0RX_ISR 		; I2C slave transmit ISR
;----------------------------------------------
			mov.b	#UCSTPIFG+UCSTTIFG, R10	; move stop and start interrupt flags to R10
			inv.b	R10
			and.b	R10, &UCB0STAT			; clear interrupt flags
			tst.w	R7
			jeq		EndISR
			bic.w	#LPM0, 0(SP)

EndISR:		reti

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
            
            .sect USCIAB0RX_VECTOR
            .short USCIAB0RX_ISR
            .sect USCIAB0TX_VECTOR
            .short USCIAB0TX_ISR
			.end
