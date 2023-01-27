;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------
; Data definition
;-------------------------------------------------------------------------------
			.data
			.align

rawTemp    .set  	R4
COUNTER    .set  	R5
BitTime    .set  	R6
BitCnt     .set  	R7
TXByte     .set  	R8
            
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
Cnfg_WDT:
   		mov.w   #WDT_ADLY_250, &WDTCTL              ; < 1 sec WDT interval
   		bis.b   #WDTIE, &IE1                        ; enable WDT interrupt

Cnfg_Pins:
    	bis.w  #BIT1+BIT2, &P1SEL                  ; select transmist and receive mode on pins
    	bic.w  #BIT3+BIT2, &P1DIR              ; input to support button switch
    	mov.w  #00h, &P1OUT

    	and.w  #~(BIT6+BIT7), &P2SEL               ; P2.6 and P2.7 are cleard to select GPIO,
                                               ; but they are also used by the external crystal
    	or.w   #BIT6+BIT7, &P2DIR
    	mov.w  #00h, &P2OUT
;-------------------------------------------------------------------------------------------------------
FaultRoutine:                                   ; if calibration data is erased
    	bis.w  #BIT0, &P1OUT                         ; red led on
    	jmp    $                                     ; TRAP

;-------------------------------------------------------------------------------------------------------
Cnfg_Clk:

    	cmp.w  	#0FFh, CALBC1_1MHZ
    	jeq   	FaultRoutine
    	cmp.w  	#0FFh, CALDCO_1MHZ
    	jeq    	FaultRoutine

    	mov.w  	#CALBC1_1MHZ, &BCSCTL1                    ; set range, DCO: digitally controlled oscillator
    	mov.w 	#CALDCO_1MHZ, &DCOCTL                     ; set DCO step + modulation,

    	mov.w  	#00h, &P1OUT

    	bis.w  	#LFXT1S_2, &BCSCTL3                        ; LFXT1 = VLO
    	bic.w  	#OFIFG, &IFG1

   		mov.w 	#0x00, &BCSCTL2
;-------------------------------------------------------------------------------------------------------
Cnfg_ADC:
        mov.w 	#INCH_10+ADC10DIV_0, &ADC10CTL1

;-------------------------------------------------------------------------------------------------------

Cnfg_TA0:
        mov.w    #OUT, &CCTL0
        mov.w    #TASSEL_2+MC_2+ID_3, &TACTL        ; SMCLK/8, continuous mode

Transmit:
        mov.w    #0Ah, BitCnt                       ; load counter, 8_data + ST/SP
        cmp.w    &CCR0, &TAR                        ; Prevent async capture
        jne      setCCR0

		mov.w    #78, BitTime
        add.w    BitTime, &CCR0                    ; Some time till first bit
        or.w     #0100h, TXByte                      ; Add mark stop bit to TXByte
        rla.w    TXByte                             ; Add space start bit, OUTMOD0 = out bit value
        mov.w    #CCIS0+OUTMOD0+CCIE, &CCTL0        ; TXD = mark = idle, capture compare input select, CCIxA
wait:   bit.b    #CCIE, &CCTL0                      ; wait for TX completion
        jz       wait

setCCR0:mov.w    &TAR, CCR0                         ; Current state of TAR counter


MainLoop:mov.w	#LPM3+GIE, SR

;----------------------------------------------------------------------------------
WDT_ISR
;-----------------------------------------------------------------------------------
		mov.w    #SREF_1+ADC10SHT_3+REFON+ADC10ON, &ADC10CTL0
        mov.w    #01h, COUNTER
again:	dec.w    COUNTER
        jnz      again
		mov.w    #ENC+ADC10SC, &ADC10CTL0
		mov.w    #95, COUNTER
repeat: dec.w    COUNTER
        jnz      repeat
		bic.w    #ADC10IFG, &ADC10CTL0
		bic.w    #ENC, &ADC10CTL0
		and.w	 #~(REFON+ADC10ON), &ADC10CTL0
		mov.w	 &ADC10MEM, rawTemp
		bis.w	 #LPM3, SR
		reti

;----------------------------------------------------------------------------------
TA0_ISR
;----------------------------------------------------------------------------------
         mov.w	 #78, BitTime
         add.w   &CCR0, BitTime
         bit.b   #CCIS0, &CCTL0
         jz      end_ISR
         cmp.w   #00h, BitCnt
	 	 jz      set_cnt
         and.w   #~CCIE, &CCTL0

set_cnt: or.w   #OUTMOD2, &CCTL0
         bit.b  #01h, TXByte
	 	 jz     end_ISR
         and.w  #~OUTMOD2, &CCTL0
         rra.b	TXByte
	 	 dec.w  BitCnt

end_ISR: reti
                                            
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
            
        	.sect  WDT_VECTOR
        	.short WDT_ISR

        	.sect TIMER0_A0_VECTOR
        	.short TA0_ISR

        	.end
;-------------------------------------------------------------------------------
