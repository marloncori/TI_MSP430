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
			bis.b	#020h, &P1DIR			; P1.5 output direction
			mov.w	#TASSEL_2+MC_2+ID_3, &TACTL
			mov.w	#SREF_1+ADC10SHT_3+ADC10SR+REF2_5V+REFON+ADC10ON, &ADC10CTL0
			mov.w	#INCH_4, &ADC10CTL1		; a/d input is on P1.4
			mov.b	&CALBC1_8MHZ, &BCSCTL1
			mov.b	&CALDCO_8MHZ, &DCOCTL
			mov		#1, &0200h

MainLoop
			clr		&TAR

ReadVoltage
			bic		#ADC10IFG, &ADC10CTL0
			bis		#ENC+ADC10SC, &ADC10CTL0	; start conversion

TestADC
			bit		#ADC10IFG, &ADC10CTL0		; check conversion status
			jz		TestADC
			cmp		#022Eh, &ADC10MEM			; 0x22E corresponds to 15 volts
			jge		GreaterThan
			inc		&0200h
			jmp		CloseSwitch

GreaterThan
			dec 	&0200h

CloseSwitch
			call	#Clipping
			mov		&0200h, R8

HoldHigh
			bis.b	#020h, &P1OUT
Again
			dec 	R8
			jnz		Again
			bic.b	#020h, &P1OUT				; clear P1.5 bit
			call	#Delay						; wait for next 60 Hz to start
			jmp		MainLoop

Clipping
			cmp		#2, &0200h
			jge		TestForTooLarge
			mov		#2, &0200h
			ret

TestForTooLarge
			cmp		#2000, &0200h
			jge		ClipHigh
			ret

ClipHigh
			mov		#2000, &0200h
			ret

; ----------------------------------------------------------------------------------------------
Delay
			cmp.w	#16600, &TAR
			jl		Delay
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

