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
Port1Setup:	BIS.B 	#0F7h, &P1DIR 		; All P1 pins but P1.3 as output
			BIS.B 	#BIT3, &P1REN 		; P1.3 Resistor enabled
			BIS.B 	#BIT3, &P1OUT 		; Set P1.3 resistor as pull-up
			BIS.B 	#BIT3, &P1IES 		; Edge sensitivity now H->L
			BIC.B 	#BIT3, &P1IFG 		; Clears any P1.3 pending IRQ

Port1IE: 	BIS.B 	#BIT3, &P1IE 		; Enable P1.3 interrupt <--(4)

Main: 		BIS.W 	#CPUOFF+GIE, SR 	; CPU off and set GIE <---(4)
			NOP 					    ; Debugger breakpoint
;---------------------------------------------------------------------------
PORT1_ISR ; Begin ISR <--------------(3)
;---------------------------------------------------------------------------
			BIC.B 	#BIT3, &P1IFG 	; Reset P1.3 Interrupt Flag
			XOR.B 	#BIT2, &P1OUT 	; Toggle LED in P1.2
			RETI 				 	; Return from interrupt <---(3)
                                            
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
            
            .sect   PORT1_VECTOR
            .short  PORT1_ISR

            .end
