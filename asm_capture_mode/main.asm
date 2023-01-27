;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; connect P1.1 to P1.3
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

setup:		mov.w	#LFXT1S_2, &BCSCTL3
			mov.b	#02h, &P1SEL
			mov.w	#CAP+CM_3+SCS+CCIE+CCIS_0, &TACCTL0 ; capture at rising - falling edges
			mov.w   #TASSEL_1+ID_3+MC_2+TACLR, &TACTL

			clr.w	R6
			clr.w	R7
			bis.w   #GIE+LPM1, SR

TA0_ISR:	inc.w	R6
			cmp.w   #1Dh, R6
			jne 	loop1
			bis.w	#TACLR, &TACTL
loop1:		cmp.w	#2Dh, R6
			jne 	loop2
			mov.w	&TACCR0, R7
			clr.w	R6
loop2:		reti
                                            

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
            
