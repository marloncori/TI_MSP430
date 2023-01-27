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
RESET       mov.w   #__STACK_END,SP        ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
STARTUP:	mov.w 	#SFE(CSTACK),SP 	   ; Initialize stack pointer
			mov.w 	#WDTPW+WDTHOLD,&WDTCTL ; Stop watchdog timer
			bis.b 	#001h,&P1DIR 		   ; Configure P1.0 as output

Main:		xor.b 	#001h,&P1OUT 		   ; Toggle P1.0
			mov.w 	#050000,R15 		   ; Use R15 as delay counter

Repeat:		dec.w 	R15 				   ; Decrement count
			jnz 	Repeat 				   ; While not zero repeat
			jmp 	Main 				   ; Do it again

;----------------------------------------------------------------------
			COMMON  INTVEC 				   ; Auto-vector table
                                            
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
            
