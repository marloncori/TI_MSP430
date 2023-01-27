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
 init:		mov.b	#0xFF, &P2DIR			; P3.0 - P3.7 as output
 			bic.b	#0x06, &P1DIR			; P1.1 and P1.2 as inputs

 check:		bit.b	#0x02, &P1IN     		; check P1.1 button state
 			jnz		check

 mainLoop:	mov.b	#0x01, R4
 subLoop:	mov.b	R4, &P2OUT

 wait:		mov.w	#065000, R5
 for:		dec.w	R5
 			jnz		for

 			bit.b	#0x04, &P1IN             ; check P1.2 button state
 			jz		check

 			rla.b	R4
 			tst.b	R4
 			jz		mainLoop
 			jmp		subLoop

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
            
