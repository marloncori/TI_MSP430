
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

init:		mov.b	#0x41, &P1DIR			; P2.0 and P2.6 as output
			mov.b 	#0x01, &P1OUT			; turn on P2.0 led

clk_config:
			mov.w   #CALBC1_1MHZ, &BCSCTL1
			mov.w   #CALDCO_1MHZ, &DCOCTL
			mov.w   #LFXT1S_2, &BCSCTL3
			bic.w   #~OFIFG, &IFG1
			mov.w   #SELM_0+DIVM_3+DIVS_3, &BCSCTL2

ta2_config:
            mov.w   #CCIE, &CCTL0
            mov.w   #0xE10, &CCR0             ; load CCR0 with 36000
            mov.w   #TASSEL_1+MC_2, &TACTL    ; for a 3 sec timing

lpm3:		bis.w	#CPUOFF+GIE, SR

;-------------------------------------------------------------------------------
TA0_ISR     ; timer A interrupt service routine (every second)
;-------------------------------------------------------------------------------
			xor.b	#0x41, &P1OUT			; toggle two leds
			add.w   #0xE10, &CCR0           ; add offset to CCR0

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
;-------------------------------------------------------------------------------
