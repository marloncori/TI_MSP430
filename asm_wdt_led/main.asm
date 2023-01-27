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
loop:		mov.b	#01h, &P1DIR
			mov.b	#00h, &P1OUT

			bis.b	#08h, &P1IE				; enable P1.3 interrupt
			bis.b	#08h, &P1IES			; edge sensitivity hi to lo

			bic.b	#08h, &P1IFG			; clear any pending IRQ
			bis.w	#LPM4+GIE, SR

;-------------------------------------------------------------------------------
PORT1_ISR   ;interrupt service routine
;-------------------------------------------------------------------------------
			mov.b	#01h, &P1OUT
			mov.w	#LFXT1S_2, &BCSCTL3		; 12 KHz VLO as clk source
			mov.w	#WDT_ARST_1000, &WDTCTL ; use WDT to reset the system
											; after 1/(12000/32768) = approx. 2.8 sec
			bic.b	#08h, &P1IFG
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
            
			.sect	PORT1_VECTOR
			.short  PORT1_ISR
