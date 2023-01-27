;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;  An example of the joint usage of the port and timer interrupts.
;
;  Here, the program counts the timer interrupts. If the sum reaches five, the red
; and green LEDs toggle. The sum is reset if the user presses the button during operation.
; This operation is done under the port ISR.
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
			mov		#0d, R5					; used as counter
			mov		#0d, R6					; used as done
			mov		#0h, R7					; used as state
			mov		#0d, R8					; used as red_count
			mov		#0d, R9					; used as green_count

			mov.b	#41h, &P1DIR
			mov.b	#08h, &P1REN
			mov.b	#09h, &P1OUT

			bis.b	#08h, &P1IE
			bis.b	#08h, &P1IES
            bic.b	#08h, &P1IFG

            mov.w	#0FFFh, &TACCR0
            mov.w	#TASSEL_2+MC_3+ID_3, &TACTL
            mov.w	#CCIE, &TACCTL0

            bis.w	#GIE, SR

loop:		cmp		#06d, R6
			jl		subloop
			clr.b	&P1OUT
			bic.w	#GIE, SR
			bis.w	#LPM4, SR

subloop:	bis.w	#LPM1, SR
			jmp		loop

P1_ISR:		tst.w	R7						; check button state
			jeq		red
			inc		R9
			jmp		ei

red:		inc 	R9
ei:			bic.b	#08h, P1IFG
			reti

TA0_ISR:	bic.w	#LPM1, 0(SP)			; toggle P1.0
			inc		R5
			cmp.w	#05d, R5
			jl		end_ISR
			xor.b	#41h, &P1OUT
			inc		R6
			mov		#0d, R5
			mov		#01d, R7
end_ISR:	reti

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
            
