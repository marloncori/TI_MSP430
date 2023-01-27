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
Init:		mov.b	#0F7h, &P1DIR			; P1.3 as input, all other pins as output
			bis.b	#08h, &P1REN			; resistor at P1.3
			bis.b	#08h, &P1OUT			; as pull-up resistor
			bis.b	#04h, &P1OUT			; turn led on, pin P1.2

Int_En:		bic.b	#08h, &P1IFG			; clears any pending IRQ
			bis.b	#08h, &P1IES			; flag set on high-to-low
			bis.b	#08h, &P1IE				; enable P1.3 interrupt flag

Main:		bis.w	#CPUOFF+GIE, SR
			NOP
;-------------------------------------------------------------------------------
PORT1_ISR ; INTERRUPT Subroutine here
;-------------------------------------------------------------------------------
			bic.b	#08h, &P1IFG			; reset interrupt flag
			xor.b	#04h, &P1OUT			; toggle led at P1.2
			reti								; return

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

