
******************************************************************************
; MSP430G2xx1 - Software shift byte
;
; Description: shift byte on P1
; ACLK = n/a, MCLK = SMCLK = default DCO ~800kHz
;
; MSP430G2211
; PHAN IT.
; September 2011
; Built with Code Composer Essentials Version: 4.2.0
;******************************************************************************
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
SetupP1: 	mov.b 	#0FFh,&P1DIR 			; P1 output
			mov.b 	#001h,R14 				; set value for R14

Mainloop:	mov.b 	R14,&P1OUT 				; R14 to P1
			call 	#Delay 					; delay
			rla 	R14 					; shift left R14
			cmp 	#000h,R14 				; cmp R14 with 0
			jne 	Mainloop 				; if R14 != 0
			mov.b 	#001h,R14 				; if R14=0 then R14 = 1
			jmp 	Mainloop 				; Again
;------------------------------------------------------------------------------
Delay:
			mov.w 	#050000,R15 			; set val to R15
Loop1: 		dec.w 	R15 					; Decrement R15
			jnz 	Loop1 					; jmp to Loop1 if !=0
			ret

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
;-------------------------------------------------------------------------------
            .end

            
