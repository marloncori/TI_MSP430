;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;  This program generates one PWM output on P1.2 using Timer_A
;  configured for up mode. The value in CCR0, 512-1, defines the PWM period and the
;  value in CCR1 the PWM duty cycles. A 75% duty cycle is on P1.2. ACLK = n/a,
;  SMCLK = MCLK = TACLK = default DCO
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

init:		bis.b	#00Ch, &P1DIR			; P1.2 and P1.3	output
			bis.b	#00Ch, &P1SEL			; P1.2, P1.3 TA1/2 options
			mov.w	#512-1, &CCR0			; PWM period, capture/compare register
			mov.w   #OUTMOD_7, &CCTL1		; CCR1 reset/set, 7*0x20 (= E0, 1110 0000)
			mov.w	#384, &CCR1				; CCR1 PWM Duty Cycle, 38400/511 = 75%
			mov.w	#TASSEL_2+MC_1, &TACTL  ; timer A control register, SMCLK, upmode
			; MC_1: 1*0x10, Timer A mode control: 1 - Up to CCR0 -> 0001 0000
			; TASSEL_2: 2*0x100u, Timer A clock source select: 2 - SMCLK -> 0010 0000 0000

main:		bis.w	#CPUOFF, SR				; CPU off, 0x0010

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
            
