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
loop:
			mov.b	#41h, &P1DIR			; P1.0 and P1.6 as output
			bic.b	#01h, &P1OUT			; P1.0 & P1.6 reset
			bis.b	#08h, &P1IE				; P1.3 interrupt enabled
			bis.b	#08h, &P1IES			; P1.3 edge sensitivity: high-low
			bic.b	#08h, &P1IFG			; P1.3 interrupt flag cleared
			clr		R5

			bis.w	#GIE, SR				; enable interrupts
			jmp		$

P1_ISR:
			inc		R5
			cmp.w	#4Dh, R5
			jeq		red_led
			cmp.w	#6Dh, R5
			jeq		green_led
			bic.b	#08h, &P1IFG
			reti							; return from ISR
                                            
red_led:	bis.b	#01h, P1OUT
			bic.b	#40h, P1OUT
			bic.b	#08h, &P1IFG
			reti							; return from ISR

green_led:	bic.b	#01h, P1OUT
			bis.b	#40h, P1OUT
			clr		R5
			bic.b	#08h, &P1IFG
			reti							; return from ISR

;------------------------------------------------------------------------------------------------
; Stack Pointer definition
;------------------------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
