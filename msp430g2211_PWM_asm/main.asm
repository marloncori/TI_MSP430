
;*******************************************************************************
; MSP430G2xx1 Demo - Timer_A, Toggle P1.0, CCR0 Up Mode ISR, DCO SMCLK
;
; Description: Toggle P1.0 using software and TA_0 ISR. Timer_A is
; configured for up mode, thus the timer overflows when TAR counts to CCR0.
; In this example CCR0 is loaded with 50000.
; ACLK = n/a, MCLK = SMCLK = TACLK = default DCO
;
; MSP430G2xx1
; -----------------
; /|\| XIN|-
; | | |
; --|RST XOUT|-
; | |
; | P1.0|-->LED
;
; D. Dang
; Texas Instruments Inc.
; October 2010
; Built with Code Composer Essentials Version: 4.2.0
;*******************************************************************************;-------------------------------------------------------------------------------
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
SetupP1: 	bis.b 	#001h,&P1DIR 			; P1.0 output
SetupC0: 	mov.w 	#CCIE,&CCTL0 			; CCR0 interrupt enabled
			mov.w 	#50000,&CCR0
SetupTA:	mov.w 	#TASSEL_2+MC_1,&TACTL 	; SMCLK, upmode

Mainloop: 	bis.w 	#CPUOFF+GIE,SR 			; CPU off, interrupts enabled
			nop 							; Required for debugger

;-------------------------------------------------------------------------------
TA0_ISR; Toggle P1.0
;-------------------------------------------------------------------------------
			xor.b 	#001h,&P1OUT 			; Toggle P1.0
			reti 							;

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
            
			.sect ".int09" 					; Timer_A0 Vector
			.short TA0_ISR

;-------------------------------------------------------------------------------
; End of application
;-------------------------------------------------------------------------------
			.end
;-------------------------------------------------------------------------------
