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
loop:		mov.w	#DIVS_3, &BCSCTL2		; SMCLK freq / 8, bit 2:1 (11)
											; 10 -> /4, 01 -> /2, 00 -> /1

			mov.w	#WDT_MDLY_32, &WDTCTL	; use WDT as 32*8 = 256 ms interval
			bis.b	#WDTIE, &IE1			; enable WDT interrupt, set bit0 of IE1 reg

			mov.b	#41h, &P1DIR
			mov.b	#01h, &P1OUT

			bis.w	#LPM1+GIE, SR			; enable interrupts and LPM

;-------------------------------------------------------------------------------
WDT_ISR ; Stack Pointer definition
;-------------------------------------------------------------------------------
			xor.b	#41h, &P1OUT
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
            
			.sect   WDT_VECTOR
			.short  WDT_ISR
