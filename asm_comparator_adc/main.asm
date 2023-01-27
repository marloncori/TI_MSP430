;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Here, the ADC10 module is used as a comparator. If the input voltage is greater
; than a predefined value, the red LED on the MSP430 LaunchPad will turn on.
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

setup:		mov.w	#ADC10SHT_2+ADC10ON+ADC10IE, &ADC10CTL0 ; 16x, enable interrupt
			mov.w	#INCH_1, &ADC10CTL1
			bis.b	#02h, ADC10AE0			; P1.1 adc10 option select
			bis.b	#01h, &P1DIR

main:		bis.w	#ENC+ADC10SC, &ADC10CTL0	; start sampling, conversion
			bis.w	#LPM0+GIE, SR				; ADC10_ISR will force exit

			bic.b	#01h, &P1OUT
			cmp.w	#01FFh, ADC10MEM			; ADC10MEM = A1 > 0.5AVcc?
			jlo		main
			bis.b	#01h, &P1OUT
			jmp		main

;-------------------------------------------------------------------------------
ADC10_ISR  ; adc10 interrupt service routine
;-------------------------------------------------------------------------------
            bic.w	#LPM0, 0(SP)				; exit LPM0 on reti
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
            
			.sect   ADC10_VECTOR
			.short  ADC10_ISR

			.end
