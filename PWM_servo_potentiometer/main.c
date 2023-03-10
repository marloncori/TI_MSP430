#include <msp430.h> 

/* drselim MSP430 & SG90 Servo
  3rd Example
  Please don't forget to give credits while using the code
  video link for the description of the code:
  https://youtu.be/V3v5ItyMKVc */

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    P1DIR |= BIT6;
    P1SEL |= BIT6;

    volatile float voltage;
    ADC10CTL0 = SREF_0|ADC10SHT_2|ADC10ON;
    ADC10CTL1 = INCH_3|SHS_0|ADC10DIV_0|ADC10SSEL_0|CONSEQ_0;
    ADC10AE0 = BIT3;  //P1.3 also above, INCH_3
    ADC10CTL0 |= ENC;

    while(1){
        ADC10CTL0 |= ADC10SC;
        while(ADC10CTL1 & ADC10BUSY);
        int memval = ADC10MEM;
        TACCR0 = 20000;
        TACCR1 = (int)(350 + (1.95*memval));
        TACCTL1 = OUTMOD_7;  //CCR1 selection reset-set
        TACTL = TASSEL_2|MC_1;   //SMCLK submain clock,upmode
        __delay_cycles(10000);
    }
}
