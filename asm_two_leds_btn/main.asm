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
setup:		mov.b	#0x41, &P1DIR			; P1.0 and P1.7 as output
 											; other pins as inputs
 			mov.w	#0xFFF,	R4				; counter1 for delay
 			mov.w	#0xFFF,	R5				; counter2 for delay

main:		bit.b	#0x08, &P1IN				; check button state at P1.3
 			jc		led_off					; jump to label is carry is set

led_on:		bic.b	#0x01, &P1OUT			; turn off led at P1.0
 			bis.b	#0x40, &P1OUT			; turn on led at P1.7
delay1:		dec.w	R4
 			jnz		delay1
 			mov.w	#0xFFF,	R4				; reload counter1
 			jmp		main

led_off:	bis.b	#0x01, &P1OUT			; turn on led at P1.0
 			bic.b	#0x40, &P1OUT			; turn off led at P1.7
delay2:		dec.w	R5
 			jnz		delay2
 			mov.w	#0xFFF,	R5				; reload counter2
 			jmp		main

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
            
