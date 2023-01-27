/* Take the multimeter and use it to measure msp430 power consumption*/

#include <msp430.h> 
#define  TXD     BIT1           // TXD on P1.1
#define  RXD     BIT2           // RXD on P1.2
#define  BitTime 13*4           // 0x0D

//==============================================================================================
volatile unsigned int TXByte;
volatile unsigned char BitCnt;
volatile long rawTemperature;
volatile long tempSet = 0;
volatile unsigned int i;

unsigned int TxHI[] = {0x48, 0x49, 0x0A, 0x08, 0x08};
unsigned int TxLO[] = {0x4C, 0x4F, 0x0A, 0x08, 0x08};
unsigned int TxIN[] = {0x49, 0x4E, 0x0A, 0x08, 0x08};

//==============================================================================================
void ConfigWDT(void);
void FaultRoutine(void);
void ConfigPins(void);
void ConfigClocks(void);
void ConfigADC(void);
void ConfigTimerA2(void);
void TransmitData(void);

//void BlinkLed(void);

//==============================================================================================

void main(void)
{
    ConfigWDT();
    ConfigPins();
    ConfigClocks();
    ConfigTimerA2();
    ConfigADC();

    while(1){
        // wait for interrupt to occur
        __bis_SR_register(LPM3_bits + GIE);
       if(!tempSet){
           tempSet = rawTemperature;        // set reference temperature
       }
       if(tempSet > (rawTemperature + 5)){    // test for lo
           P1OUT = BIT6;                    // green led on
           P1OUT &= ~BIT0;                   // red led off
           for(i = 0; i < 5; i++){
              TXByte = TxLO[i];
              TransmitData();
           }
       }

       if(tempSet < (rawTemperature - 5)){    // test for hi
           P1OUT = BIT0;                    // green led off
           P1OUT &= ~BIT6;                   // red led on
           for(i = 0; i < 5; i++){
              TXByte = TxHI[i];
              TransmitData();
           }
       }


       if((tempSet <= rawTemperature + 2) && (tempSet >= rawTemperature - 2)){
                                             // test for in range value
                                             // both leds are off
           P1OUT &= ~(BIT0 + BIT6);
           for(i = 0; i < 5; i++){
              TXByte = TxIN[i];
              TransmitData();
           }
       }
     }
}

//==============================================================================================
void ConfigWDT(void){
   // I am using the VLO as clock source
   WDTCTL = WDT_ADLY_250; // < 1 sec WDT interval
   IE1 |= WDTIE;          // enable WDT interrupt
}

void FaultRoutine(void){
    P1OUT = BIT0;                          // red led on
    while(1){}                             //TRAP
}

void ConfigPins(void){
   P1SEL |= TXD + RXD;      // select transmist and receive mode on pins
   P1DIR &= ~(BIT3 + RXD);          // input to support button switch
   P1OUT = 0x00;

   P2SEL &= ~(BIT6 + BIT7); // P2.6 and P2.7 are cleard to select GPIO,
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

    BCSCTL2 = 0x00;
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

void ConfigTimerA2(void){
    CCTL0 = OUT;                    // TXD Idle as mark
    TACTL = TASSEL_2 + MC_2 + ID_3; // SMCLK/8, continuous mode
}

void TransmitData(void){
  BitCnt = 0xA;                     // Load bit counter, 8_data + ST/SP
  while(CCR0 != TAR){               // Prevent async capture
      CCR0 = TAR;                  // Current state of TA counter
  }
  CCR0 += BitTime;                 // Some time till first bit
  TXByte |= 0x100;                 // Add mark stop bit to TXByte
  TXByte = TXByte << 1;            // Add space start bit, OUTMOD0 = out bit value
  CCTL0 = CCIS0 + OUTMOD0 + CCIE;  // TXD = mark = idle, capture compare input select, CCIxA
  while(CCTL0 & CCIE){}             // wait for TX completion
}

//==============================================================================================
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void){
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
    ADC10CTL0 = ENC + ADC10SC;
    /* LPM0 with interrupts enabled
       * sampling and conversion start
       * According to the USER'S Guide, I should allow
       *  13 ADC10 clock cycles before reading the conversion result
       *  thirteen clock cycles of the 5 MHz ADC10 internal oscillator is 2.6 us
    */
      __delay_cycles(100);

    ADC10CTL0 &= ~ADC10IFG;
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 &= ~(REFON+ADC10ON);

    rawTemperature = ADC10MEM;          // read conversion result

     __bis_SR_register_on_exit(LPM3_bits);
     // this line of code clears the bits the Status Register
}
//==============================================================================================
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){

   CCR0 += BitTime;                   // Add offset to CCR0
   if(CCTL0 & CCIS0){               // TX on CCI0B?
       if(BitCnt == 0){
           CCTL0 &= ~CCIE;           // all bits TXed, disabled interrupt
       }
       else {
           CCTL0 |= OUTMOD2;          // tx space
           if(TXByte & 0x01){
               CCTL0 &= ~OUTMOD2;
               TXByte = TXByte >> 1;  // tx mark
               BitCnt--;
           }
       }
   }
}
//==============================================================================================
