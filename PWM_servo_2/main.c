#include <msp430.h> 
#define PWM_TIME 1500000

/*drselim MSP430 & SG90 Servo
 1st Example
 Please don't forget to give credits while using the code
 video link for the description of the code:
 https://youtu.be/V3v5ItyMKVc */

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    //PWM period
    P1DIR |= BIT6;
    P1SEL |= BIT6;  //selection for timer setting

    while(1) {
      TACCR0 = 20000;  //PWM period
      TACCR1 = 350;  //CCR1 PWM Duty Cycle  !min 350 max 2600 angle 190,
      //350 2350-180 degrees

      TACCTL1 = OUTMOD_7;     //CCR1 selection reset-set
      TACTL = TASSEL_2|MC_1;  //SMCLK submain clock, up mode
      __delay_cycles(PWM_TIME);

      TACCR1 = 2600;
      TACCTL1 = OUTMOD_7;  //CCR1 selection reset-set
      TACTL = TASSEL_2| MC_1; //SMCLK submain clock, up mode
      __delay_cycles(PWM_TIME);
    }
}
