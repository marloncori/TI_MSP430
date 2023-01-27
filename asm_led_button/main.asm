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
init:		mov.b	#0x42, &P1DIR			; P1.6 and P1.1 as outputs
			mov.b	#0x08, &P1REN			; push button on P1.3
			mov.b	#0x08, &P1OUT			; enable pull-up resistor on P1.3
			mov.w	#0xFFFF, R4				; value to be used in delays
			bis.b	#0x40, &P1OUT			; turn on led at P1.6

loop:		bit.b	#0x08, &P1IN			; check button state at P1.3
			jnz		btn_press				; jump to that label if 1 ; try it also with "jnc"
			bis.b	#0x40, &P1OUT			; turn on led at P1.6
			jmp		loop

btn_press:	mov.w	#500, R5
debounce:	dec.w   R5
			jnz		debounce
			bic.b	#0x40, &P1OUT			; turn off led at P1.6
			bis.b	#0x02, &P1OUT			; turn on led at P1.1
wait:		dec.w	R4
			jnz		wait
			mov.w	#0xFFFF, R4				; reset user register for next call
			mov.w	#500, R5
			jmp 	loop

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
            
