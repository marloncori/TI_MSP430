;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
; ------------------------------------------------------------------------------
;   Toggle P1.0 using software and TA_0 ISR. Toggles every 50000
; SMCLK cycles. SMCLK provides clock source for TACLK During the TA_0 ISR,
; P1.0 is toggled and 50000 clock cycles are added to CCR0. TA_0 ISR is
; triggered every 50000 cycles. CPU is normally off and used only during TA_ISR.
; ACLK = n/a, MCLK = SMCLK = TACLK = def ault DCO
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

SetupP1: 	bis.b 	#41h, &P1DIR 			; P1.0 output
			mov.b   #40h, &P1OUT			; turn P1.6 led

SetupAClk:  mov.w   #CALBC1_1MHZ, &BCSCTL1
			mov.w   #CALDCO_1MHZ, &DCOCTL
			or.w    #LFXT1S_2, &BCSCTL3
			bic.w   #~OFIFG, &IFG1
			or.w    #SELM_0+DIVM_3+DIVS_3, &BCSCTL2

SetupTA0: 	mov.w 	#CCIE, &CCTL0 			; CCR0 interrupt enabled
			mov.w 	#4B0h, &CCR0            ; load CCR0 with 1200
	     	mov.w 	#TASSEL_1+MC_2, &TACTL 	; TASSEL_2: SMCLK, continuous mode
											; TASSEL_1: ACLK
Mainloop: 	bis.w 	#CPUOFF+GIE, SR			; CPU off, interrupts enabled
			;nop 							; Required only for debugger

;---------------------------------------------------------------------------
TA0_ISR; Toggle P1.0 and P1.6 every second
;---------------------------------------------------------------------------
			xor.b 	#41h, &P1OUT 			; Toggle P1.0
			add.w 	#4B0h, &CCR0 			; Add 1200 Offset to CCR0
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
            
			.sect   TIMER0_A0_VECTOR
			.short  TA0_ISR

			.end
; ------------------------------------------------------------------------------

