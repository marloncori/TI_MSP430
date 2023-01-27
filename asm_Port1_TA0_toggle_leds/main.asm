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
Variables:	mov		#0d, R5					; used as count
            mov		#0d, R6					; used as done
            mov		#0d, R7					; used as state
            mov		#0d, R8					; used as redCount
            mov		#0d, R9                 ; used as greenCount

LedInit:	mov.b	#41h, &P1DIR
			mov.b	#08h, &P1REN
			mov.b	#09h, &P1OUT

Interrupt:	bis.b	#08h, &P1IE
			bis.b	#08h, &P1IES
			bic.b	#08h, &P1IFG

Timer:		mov.w	#0FFFFh, &TACCR0
			mov.w	#TASSEL_2+MC_3+ID_3, &TACTL
			mov.w	#CCIE, &TACCTL0

Enable:		bis.w	#GIE, SR

Loop:		cmp		#06d, R6
			jl		SubLoop
			clr.b	P1OUT
			bic.w	#GIE, SR
			bis.w	#LPM4, SR

SubLoop:	bis.w	#LPM1, SR
			jmp		Loop

;-------------------------------------------------------------------------------
P1_ISR		; check for the button state
;-------------------------------------------------------------------------------
			tst.w	R7
			jeq		Red
			inc		R9
			jmp		Ei

Red:		inc		R8

Ei:			bic.b	#08h, &P1IFG
			reti

;-------------------------------------------------------------------------------
TA0_ISR		; toggle led at P1.0
;-------------------------------------------------------------------------------
			bic.w	#LPM1, 0(SP)

			inc		R5
			cmp.w	#05d, R5
			jl		EndISR

			xor.b	#41h, &P1OUT
			inc		R6

			mov		#0d, R5
			xor		#01d, R7

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
            
			.sect	PORT1_VECTOR
			.short  P1_ISR

			.sect	TIMER0_A0_VECTOR
			.short	TA0_ISR

			.end
;------------------------------------------------------------------------------
