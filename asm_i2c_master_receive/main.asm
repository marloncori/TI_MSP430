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
LedInit:	bis.b	#24h, &P1DIR
			bis.b	#0C4h, &P1SEL
			bis.b	#0C0h, &P1SEL2
			bis.b	#08h, &P1IE
			bis.b	#08h, &P1IES
			clr.b	P1IFG

I2C_Config:	bis.b	#UCSWRST, &UCB0CTL1		 ; adjust i2c mode, enable SW reset
			mov.b	#UCMST+UCMODE_3+UCSYNC, &UCB0CTL0
			; clock mode 3, MSB first, 8-bit I2C master, synchronous mode
			mov.b	#UCSSEL_2+UCSWRST, &UCB0CTL1
			; keep SW reset and use SMCLK
			mov.w	#48h, &UCB0I2COA		 ; slave address
			bic.b	#UCSWRST, &UCB0CTL1		 ; clear reset, resume operation
			bis.b	#UCB0RXIE, IE2

PWM_Setup:	clr.w	TACCR1					 ; adjust the PWM
			mov.w	#03E7h, &TACCR0
			mov.w	#OUTMOD_7, &TACCTL1
			mov.w	#TASSEL_2+MC_1+ID_3, &TACTL

			mov.w	#0h, R6					; reset start enable used for controlling I2C start/stop
			clr.w	R8						; R8 used for getting the high byte of data
			clr.w	R9						; R8 used for getting the low byte of data

			bic.b	#20h, &P1OUT
			bis.b	#20h, &P1OUT			; reset I2C slave

Interrupts:	bis.w	#GIE, SR

MainLoop:	mov.w	#2h, R7

StopCond:	bit.b	#UCTXSTP, &UCB0CTL1
			jne		StopCond				; ensure stop condition is sent
			cmp.w	#1h, R6					; check if R6 is set
			jne 	MainLoop
			bis.b	#UCTXSTT, &UCB0CTL1		; I2C receiver, start condition
			bis.w	#LPM0, SR
			jmp		MainLoop

;----------------------------------------------
USCIAB0TX_ISR 		; SPI slave receive ISR
;----------------------------------------------
			dec.w	R7
			tst.w	R7
			jne		LastByte
			mov.b	UCB0RXBUF, R8			; copy received data (low byte) to LSB of R8
			cmp.w	#1h, R7
			jne		EndISR
			bis.b	#UCTXSTP, &UCB0CTL1		; I2C stop condition
			jmp		EndISR

LastByte:	mov.b	UCB0RXBUF, R9			; copy the receive data (low byte) into R9
			swpb	R8
			and.w	#0F00h, R8				; and R8 with 0xF00, high byte nums are on the bits
			bis.w	R8, R9
			mov.w	R9, TACCR1				; copy received 2 bytes of data to TACCR1
			clr.w	R6						; clear start enable
			clr.w	R8
			clr.w	R9
			bic.w	#LPM0, 0(SP)

EndISR:		reti

;---------------------------------------------
P1_ISR
;---------------------------------------------
			mov.w	#1h, R6					; set start enable
			clr.b	P1IFG
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
            
			.sect	USCIAB0TX_VECTOR
			.short	USCIAB0TX_ISR

			.sect 	PORT1_VECTOR
			.short	P1_ISR

			.end
