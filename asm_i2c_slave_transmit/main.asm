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

Data:		mov.w	#0000h, &200h
			mov.w	#00FAh, &202h
			mov.w	#01F4h, &204h
			mov.w	#02EEh, &206h
			mov.w	#03E7h, &208h

LedInit:	bis.b	#0C0h, &P1SEL
			bis.b	#0C0h, &P1SEL2

I2C_Setup: 	bis.b	#UCSWRST, &UCB0CTL1		     ; adjust I2C mode, enable SW reset
			mov.b	#UCMODE_3+UCSYNC, &UCB0CTL0  ; I2C slave, synchronous mode
			mov.w	#48h, &UCB0I2COA			 ; own address
			mov.b	#UCSWRST, &UCB0CTL1 		 ; clear reset, resume operation
			bis.b	#UCSTPIE+UCSTTIE, &UCB0I2CIE ; enable STT and STP interrupts
			bis.b	#UCB0TXIE, &IE2				 ; enable TX interrupt
			mov.w	#0200h, R5				     ; write the start address of Tx data into R5
			clr		R10						; clear r10 used for checking which element
											; is transmitted
			bis.w	#GIE, SR

MainLoop:	cmp.w	#5h, R10			; check if all five Data elements have been sent
			jne     SubLoop				; if they are sent, reload R10 with first element`s addr
			sub.w 	#10, R5
			clr		R10

SubLoop:	clr.w	R7
			bis.w	#LPM0, SR
			jmp		MainLoop

;-------------------------------------------------
USCIAB0TX_ISR
;-------------------------------------------------
			bit.w	#1h, R7				; check if R7 is odd
			jne 	OddByte				; if it is jump to OddByte
			mov.w	@R5, R8				; write the array of incoming element to R8
			swpb	R8
			mov.b	R8, &UCB0TXBUF		; load tx buffer with the high byte of incoming element

OddByte:	cmp.w	#1h, R7
			jne 	IncByteNum
			mov.w	@R5+, R9
			mov.b	R9, &UCB0TXBUF		; load tx buffer with low byte of incoming element
			inc.w	R10					; increase R10 to indicate that one element is sent

IncByteNum:	inc.w 	R7
			reti

;-------------------------------------------------
USCIAB0RX_ISR							; check restart or stop conditions
;-------------------------------------------------
			mov.b	#UCSTPIFG+UCSTTIFG, R11 ; copy stop and start interrupt flags to R11
			inv.b	R11					; invert R11
			and.b	R11, &UCB0STAT		; clear interrupt flags
			tst.w	R7					; check if R7 equals zero
			jne		EndISR
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
            
            .sect 	USCIAB0TX_VECTOR
            .short 	USCIAB0TX_ISR

            .sect 	USCIAB0RX_VECTOR
            .short 	USCIAB0RX_ISR

            .end
