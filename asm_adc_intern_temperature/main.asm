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
AdcInit:	mov.w	#CONSEQ_3+INCH_10+ADC10DIV_7, &ADC10CTL1
			mov.w	#SREF_1+MSC+ADC10SHT_3+REFON, &ADC10CTL0
			bis.w	#ADC10ON+ADC10IE, &ADC10CTL0
			mov.b	#10h, &ADC10DTC1		; 16 conversions

LedSetup:	bis.b	#41h, P1DIR
			bic.b	#41h, P1OUT

MainLoop:	bic.w	#ENC, &ADC10CTL0
			mov.w	#0200h, &ADC10SA        ; data buffer start
			bis.w	#ENC+ADC10SC, &ADC10CTL0 ; sampling and conversion start
			bis.w	#LPM0+GIE, SR
			call	#Average

			cmp.w	#02E2h, R6				; is temperature > 27C?
			jlo		Less
			bis.b	#01h, P1OUT
			jmp		MainLoop

Less:		bis.b	#40h, P1OUT
			jmp		MainLoop

Average:	mov.w	#0200h, R5				; set as a pointer
			mov.w	#0000h, R6				; set as sum
			mov.w	#04d, R7				; set as counter

Total:		add.w	@R5, R6
			incd.w	R5
			cmp.w	#0220h, R5
			jlo		Total
Repeat:		rra.w	R6
			dec.w	R7
			jnz		Repeat
			ret

;-------------------------------
ADC10_ISR
;-------------------------------
			bic.b	#41h, P1OUT
			bic.w	#LPM0, 0(SP)
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

            .sect	ADC10_VECTOR
            .short	ADC10_ISR
			.end
