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

init:
			mov.b	#0x02, &P1DIR			; set led at P1.2 as output
			bic.b	#0x02, &P1OUT			; make sure led is turned off
			mov.b	#0x04, &P2REN			; enable P2.3 as input
; ==========================================================================
loop:
			bit.b	#0x04, &P2IN			; check ir sensor state at P2.3
			jnz		motion
no_motion:
			bic.b	#0x02, &P1OUT
			jmp		loop
; ==========================================================================
motion:
			mov.b	#0xFF, R5
delay:		dec.b	R5						; debounce time
			jnz		delay
			bis.b	#0x02, &P1OUT			; turn on led
			jmp		loop					; go back to main routine

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
            
