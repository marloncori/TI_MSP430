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
Var_Init:	mov		#0d, R5					; used as counter
			mov		#0d, R6				    ; used as contorl flag

Pin_Config:
			mov.b	#0FEh, &P1DIR
			mov.b	#010h, &P1OUT

Timer_Config:
			mov.b	#CCIE, &TACCTL0
			mov.b	#OUTMOD_7, &TACCTL1
			mov.b	#MC_0, &TACTL
			mov.w	#49d, &TACCR0
			mov.w	#25d, &TACCR1

ADC_Config:
			mov.w	#ADC10ON+SREF_0+ADC10SHT_3, &ADC10CTL0
			mov.w	#INCH_0+ADC10SSEL_0+ADC10DIV_0+SHS_0+CONSEQ_0, &ADC10CTL1
			mov.b	#BIT0, &ADC10AE0
			or.b	#ENC, &ADC10CTL0

			bis.b	#GIE+CPUOFF, SR

Main_Loop:  or.w	#ADC10SC, &ADC10CTL0
			bit.b	#ADC10BUSY, &ADC10CTL1
			jnz		Main_Loop
			call	#Dispense_Paper

Dispense_Paper:
			cmp		#0d, R5
			jeq		ADC_Read
			ret

ADC_Read:   cmp		#0300h, &ADC10MEM
			jlo		Start_Motor
			jmp		Main_Loop

Start_Motor:
			bis.b	#04h, &P1SEL
			mov.w	#MC_1+ID_2+TASSEL_2+TACLR, &TACTL
			mov		#1d, R6
			jmp		Main_Loop
;-------------------------------------------------------------------------------
TA0_ISR  ; timer A0 interrupt service routine
;-------------------------------------------------------------------------------
			inc		R5
			cmp		#20000d, R5
			jne		EndISR

Stop_Motor: bic.b	#04h, &P1SEL
			mov.b	#MC_0, &TACTL
			mov		#0d, R6
			mov		#0d, R5

EndISR:		reti
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

       		.sect   TIMER0_A0_VECTOR
       		.short  TA0_ISR

       		.end
