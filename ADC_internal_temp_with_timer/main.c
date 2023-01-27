#include <msp430.h> 

/*---------------------------------------------------------------------------------------------
   GLOBAL VARIABLES
*--------------------------------------------------------------------------------------------*/
volatile long rawTemperature;
volatile unsigned char i;
/*---------------------------------------------------------------------------------------------
   FUNCTION PROTOTYPES
---------------------------------------------------------------------------------------------*/
void StopWatchdog(void);
void FaultRoutine(void);
void InitClocks(void);
void ConfigTimerA2(void);
void ConfigADC(void);

/*---------------------------------------------------------------------------------------------
  MAIN ROUTINE
---------------------------------------------------------------------------------------------*/
void main(void)
{
    StopWatchdog();
    InitClocks();
    ConfigTimerA2();
    ConfigADC();

    _BIS_SR(GIE);

    while(1){
        P1OUT |= BIT2;
        for(i= 100; i > 0; i--){}           // blink it twice per second
        P1OUT &= ~BIT2;
        for(i= 5000; i > 0; i--){}
    }
}

/*---------------------------------------------------------------------------------------------
  FUNCTION DECLARATIONS
---------------------------------------------------------------------------------------------*/
void StopWatchdog(void){
   WDTCTL = WDTPW | WDTHOLD;
}
//------------------------------------------------------------------------------------
void FaultRoutine(void)
{
    P1OUT = BIT0;                          // red led on
    while(1){}                             //TRAP
}
//------------------------------------------------------------------------------------
void InitClocks(void){
                                           // setting the DCO to 1 MHz frequency
    if(CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF)
    {
        FaultRoutine();                    // if calibration data is erased
    }

    BCSCTL1 = CALBC1_1MHZ;                 // set range, DCO: digitally controlled oscillator
    DCOCTL  = CALDCO_1MHZ;                 // set DCO step + modulation,
                                           // VLO: very low frequency oscillator

    P1DIR = BIT6 + BIT0;
    P1OUT = 0x01;

    //BCSCTL3 |= LFXT1S_0 + XCAP_3;  --> LFXT1 = 32675 crystal
    BCSCTL3 |= LFXT1S_2;                   // LFXT1 = VLO
    IFG1 &= ~OFIFG;

    //BCSCTL2 |= SELM_3 + DIVM_3;
    BCSCTL2 |= SELM_0 + DIVM_3 + DIVS_3;   // MCLK = DCO/8
    if(IFG1 & OFIFG) {
       FaultRoutine();
    }
}

void ConfigTimerA2(void){
  // the timer real name is TIMER A3
  CCTL0 = CCIE;                      /* enables counter compare register zero interrupt
                                        in the capture compare control register */
  CCR0 = 12000;                     // interrupt generated every second
  TACTL = TASSEL_1 + MC_2;          // ACLK as clock source, continuous counting mode
                                    /* the ACLK is running at 12 KHz, 12000 cycles are needed
                                       for it to go off very second */
}

//------------------------------------------------------------------------------------
void ConfigADC(void){
                   //or ADC10DIV_3;
   ADC10CTL1 = INCH_10 | ADC10DIV_0;
   /*INCH: input channel, _10: it selects the internal temperature sensor input
    *  channel
    * ADC10DIV_0: selects divided by 1 as the ADC10 Clk,
    * but ADC10DIV_1: divided by 2, ADC10DIV_2: divided by 4, ADC10DIV_3: divided by 8
    * It can be the internal 5 MHZ oscillator, or the MCLK, SMLK, ACLK
    * ACD10CLK is the default oscillator (internal 5 MHZ osc.)
    * * */
}

/*---------------------------------------------------------------------------------------------
   INTERRUPT SERVICE ROUTINE
---------------------------------------------------------------------------------------------*/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_A_ISR(void){

    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON;
    /*SREF_1: selects the range from Vss to Vref, that is ideal for the temp. sensor
     * ADC10SHT_3: ADC10 sample hold time three, as the maximum sample hold
     *   time, ideal for the sensor
     * REFON: it turns the reference generator on, we will wait for the reference voltage
     *  to settle after this line, though.
     * ADC10ON turns the analog to digital conversion peripheral on
     * ADC10IE enables the ADC10 interrupt
     * */                          // DCO/8 = 125 KHz
    // The needed time for the ADC to settle is less then 30 us
    __delay_cycles(5);            /* 1000 cycles = 8 ms, 5 = 40 us*/
    ADC10CTL0 |= ENC + ADC10SC;    /* LPM0 with interrupts enabled*/
                                   /* sampling and conversion start */
    /* According to the USER'S Guide, I should allow
     *  13 ADC10 clock cycles before reading the conversion result
     *  thirteen clock cycles of the 5 MHz ADC10 internal oscillator is 2.6 us
     */
    P1OUT |= BIT6;              // turn on green led
    __delay_cycles(100);

    /*  When the conversion is complete both the encoder and the reference should
    *  be turned off. The encoder bit must be off in order to change the reference bit.
    *
    *  It is a two step process
    * */
    ADC10CTL0 &= ~ENC;                  // disable ADC10 before reading data
    ADC10CTL0 &= ~(REFON + ADC10ON);

    rawTemperature = ADC10MEM;          // read conversion result

    P1OUT &= ~BIT6;             // turn off green led
    CCR0 += 12000;              // reload the counter for a new 1 second interrupt
                                // it is settled 12 cycles into the future
}

/*---------------------------------------------------------------------------------------------
   END OF APPLICATION
---------------------------------------------------------------------------------------------*/
