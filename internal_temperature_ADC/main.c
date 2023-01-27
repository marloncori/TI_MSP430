#include <msp430.h> 

/*---------------------------------------------------------------------------------------------
   GLOBAL VARIABLES
*--------------------------------------------------------------------------------------------*/
volatile long rawTemperature;

/*---------------------------------------------------------------------------------------------
   FUNCTION PROTOTYPES
---------------------------------------------------------------------------------------------*/
void StopWatchdog(void);
void FaultRoutine(void);
void InitClocks(void);
void ConfigADC(void);
void StartADC_Conversion(void);
void TurnOffADC(void);
void ReadConversionResult(void);
void GreenLedOn(void);
void GreenLedOff(void);

/*---------------------------------------------------------------------------------------------
  MAIN ROUTINE
---------------------------------------------------------------------------------------------*/
void main(void)
{
	StopWatchdog();
	InitClocks();

	while(1){
	   ConfigADC();
	   StartADC_Conversion();
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
    P1OUT = 0x01;                          // red led on
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

    P1DIR = 0x41;
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
   ADC10CTL0 |= ENC + ADC10SC;   /* LPM0 with interrupts enabled*/
                                  /* sampling and conversion start */
   /* According to the USER'S Guide, I should allow
    *  13 ADC10 clock cycles before reading the conversion result
    *  thirteen clock cycles of the 5 MHz ADC10 internal oscillator is 2.6 us
    */

}
//-------------------------------------------------------------------------------------
void TurnOffADC(void){
    /*  When the conversion is complete both the encoder and the reference should
    *  be turned off. The encoder bit must be off in order to change the reference bit.
    *
    *  It is a two step process
    * */
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 &= ~(REFON + ADC10ON);
}
//--------------------------------------------------------------------------------------
void ReadConversionResult(void){
   rawTemperature = ADC10MEM;
}
//--------------------------------
void GreenLedOn(void){
   P1OUT = 0x40;
}
//--------------------------------
void GreenLedOff(void){
   P1OUT = 0x00;
}
//--------------------------------
void StartADC_Conversion(void){
    GreenLedOn();
    _delay_cycles(100);

    TurnOffADC();
    ReadConversionResult();

    GreenLedOff();
    _delay_cycles(125000);   // this roughly 1 sec, since the frequency is 125 KHz
}
/*---------------------------------------------------------------------------------------------
   END OF APPLICATION
---------------------------------------------------------------------------------------------*/
