/* Take the multimeter and use it to measure msp430 power consumption*/

#include <msp430.h> 

//==============================================================================================
volatile long rawTemperature;

//==============================================================================================
void ConfigWatchdog(void);
void FaultRoutine(void);
void ConfigPins(void);
void ConfigClocks(void);
void ConfigTimer2(void);
void ConfigADC(void);
//void BlinkLed(void);

//==============================================================================================

void main(void)
{
    ConfigWatchdog();
    ConfigPins();
    ConfigClocks();
    ConfigTimer2();
    ConfigADC();

    while(1){
        // wait for interrupt to occur
        __bis_SR_register(LPM3_bits + GIE);
        /* this turns on the interrupt and places the device
         * in the low power mode 3, i.e. CPU, MCLK, SMCLK, DCO are all off,
         * only the ACLK, sourced by the VLO is still running in this mode.
         * This is running the preset timer.
         */
    }

}

//==============================================================================================
void ConfigWatchdog(void){
   WDTCTL = WDTPW | WDTHOLD;
   // stop watchdog timer
}

void FaultRoutine(void){
    P1OUT = BIT0;                          // red led on
    while(1){}                             //TRAP
}

void ConfigPins(void){
   P1DIR = ~BIT3;          // input to support button switch
   P1OUT = 0x00;

   P2SEL = ~(BIT6 + BIT7); // P2.6 and P2.7 are cleard to select GPIO,
                           // but they are also used by the external crystal
   P2DIR |= BIT6 + BIT7;
   P2OUT = 0x00;
}

void ConfigClocks(void){
    // setting the DCO to 1 MHz frequency
    if(CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF){
        FaultRoutine();                    // if calibration data is erased
    }

    BCSCTL1 = CALBC1_1MHZ;                 // set range, DCO: digitally controlled oscillator
    DCOCTL = CALDCO_1MHZ;                 // set DCO step + modulation,

    P1OUT = 0x00;

    BCSCTL3 |= LFXT1S_2;                   // LFXT1 = VLO
    IFG1 &= ~OFIFG;

    BCSCTL2 |= SELM_0 + DIVM_3 + DIVS_3;   // MCLK = DCO/8
    if(IFG1 & OFIFG) {
       FaultRoutine();
    }
}

void ConfigTimer2(void){
    /* the ACLK is running at 12 KHz, so 12000 cycles are
     * needed for it to go off every second */
    CCTL0 = CCIE;                     /* enables counter compare register zero interrupt
                                        in the capture compare control register */
    CCR0  = 1200 * 3;                   // interrupt generated every 3 seconds
    TACTL = TASSEL_1 + MC_2;           // ACLK as clock source, continuous counting mode
}

void ConfigADC(void){

    ADC10CTL1 = INCH_10 + ADC10DIV_0;
    /*INCH: input channel, _10: it selects the internal temperature sensor input
       *  channel
       * ADC10DIV_0: selects divided by 1 as the ADC10 Clk,
       * but ADC10DIV_1: divided by 2, ADC10DIV_2: divided by 4, ADC10DIV_3: divided by 8
       * Its clock can be the internal 5 MHZ oscillator, or the MCLK, SMLK, ACLK
       * ACD10CLK is the default oscillator (internal 5 MHZ osc.)*/

}

/*void BlinkLed(void){
    //P1OUT |= BIT2;
    for(i= 100; i > 0; i--){}
    // blink it twice per second
    //P1OUT &= ~BIT2;
    for(i= 5000; i > 0; i--){}
}*/

//==============================================================================================
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){
    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
        /*SREF_1: selects the range from Vss to Vref, that is ideal for the temp. sensor
         * ADC10SHT_3: ADC10 sample hold time three, as the maximum sample hold
         *   time, ideal for the sensor
         * REFON: it turns the reference generator on, we will wait for the reference voltage
         *  to settle after this line, though.
         * ADC10ON turns the analog to digital conversion peripheral on
         * ADC10IE enables the ADC10 interrupt
         * */                          // DCO/8 = 125 KHz
        // The needed time for the ADC to settle is less then 30 us
        __delay_cycles(4);            /* 1000 cycles = 8 ms, 5 = 40 us*/
    ADC10CTL0 = ENC + ADC10SC;
    /* LPM0 with interrupts enabled
       * sampling and conversion start
       * According to the USER'S Guide, I should allow
       *  13 ADC10 clock cycles before reading the conversion result
       *  thirteen clock cycles of the 5 MHz ADC10 internal oscillator is 2.6 us
    */
    P1OUT |= BIT6;              // turn on green led
    __delay_cycles(100);
    CCR0 += 36000;              // reload the counter for a new 3 second interrupt
                                  // it is settled 12 cycles into the future
     __bis_SR_register_on_exit(LPM3_bits);
     // this line of code clears the bits the Status Register
}
//==============================================================================================
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void){
      /*  When the conversion is complete both the encoder and the reference should
          *  be turned off. The encoder bit must be off in order to change the reference bit.
          *
          *  It is a two step process
          * */
      ADC10CTL0 &= ~ADC10IFG;
      ADC10CTL0 &= ~ENC;
      ADC10CTL0 &= ~(REFON+ADC10ON);

      rawTemperature = ADC10MEM;          // read conversion result
       __bis_SR_register_on_exit(LPM3_bits);
       // this line of code clears the bits the Status Register
}

//==============================================================================================


