;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;  Below, we provide a sample assembly codes for the usage of the Comparator_A+
; module. The input from CA1 is compared with the internal reference voltage,
; which is 0.25 × VCC here. If the input voltage is greater than this reference voltage,
; the green LED (connected to the P1.6 on the MSP430 LaunchPad) will turn on. Otherwise,
; the red LED (connected to the P1.0 on the MSP430 LaunchPad)
; will turn on.
;
;   This application can be taken as a simple battery check system. In Listing 11.1,
; we first set the comparator parameters. Then we check the input values and turn on
; the appropriate LED in an infinite loop. To do so, we have to check the
; register CACTL2 since we cannot reach the CAOUT bit alone.
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
config:		bis.b	#41h, &P1DIR
			mov.b	#CARSEL+CAREF_1+CAON, &CACTL1
			mov.b	#P2CA4, &CACTL2

main:		bit.b	#CAOUT, &CACTL2
			jz		red_led

green_led:	bis.b	#40h, &P1OUT
			bic.b	#01h, &P1OUT
			jmp 	main

red_led:	bic.b	#40h, &P1OUT
			bis.b	#01h, &P1OUT
			jmp 	main

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
            
            .end
;------------------------------------------------------------------------------
