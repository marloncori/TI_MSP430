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
			; button is hardware debounced (capacitor + pull resistor)
setup:		bis.b	#0F7h, &P1DIR			; all P1 pins but P1.3 as output
			bis.b	#04h, &P1REN			; P1.3 resistor enabled
			bis.b	#04h, &P1OUT			; P1.3 pull-up resistor
			bis.b	#04h, &P1IES			; P1.3 edge sensitivity now H->L
			bic.b	#04h, &P1IFG			; clears P1.3 pending IRQ
port1_int:  bis.b	#04h, &P1IE				; enable interrupt

main:		bis.w	#CPUOFF+GIE,SR			; CPU off and set GIE <---- (4)
			nop

;-------------------------------------------------------------------------------
; PORT1_ISR ; Begin ISR <--- (3)
;-------------------------------------------------------------------------------
            bic.b	#04h, &P1IFG			; reset interrupt flag
            xor.b	#02h, &P1OUT			; toggle led in P1.2
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
            
