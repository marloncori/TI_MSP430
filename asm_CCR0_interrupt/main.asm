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
LedInit:	bis.b 	#BIT0, &P1DIR   		; output pin P1.0
			bic.b 	#BIT0, &P1OUT   		; red LED off

TimerSetup:	mov.w	#CCIE, &TACCTL0   		; CCR0 interrupt
			mov.w   #10000, &TACCR0         ; Load upper bound
			mov.w 	#MC_1+TASSEL_1, &TACTL  ; up mode, ACLK

EnableInt:	bis.w 	#LPM0+GIE,SR 			; low power mode 0, interrupts
			nop 							; only to sync debugger

;--------------------------------------------------------------------------
; TACCR0 ISR 	; Interrupt Service Routine
;--------------------------------------------------------------------------
TACCR0 ISR 	xor.b 	#BIT0, &P1OUT 			; toggle LED
											; CCIFG automatically reset
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
            
			.sect	TIMER0_A0_VECTOR
			.short  TACCR0_ISR

			.end
;-------------------------------------------------------------------------------
