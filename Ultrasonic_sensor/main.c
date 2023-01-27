#include <msp430.h> 

/* Program on how interface HSRC-04 with the msp430g2553
 * to calculate distance to objects timer A interrupt service routine*/

int miliseconds;
int distance;
long duration;

void main(void)
{
  BCSCTL1 = CALBC1_1MHZ;                // basic clock selection control register
  DCOCTL = CALDCO_1MHZ;                 // submain clock at 1mhz
  WDTCTL = WDTPW + WDTHOLD;             // Stop WDT

  CCTL0 = CCIE;                        // CCR0 interrupt enabled
  CCR0 = 1000;                         // 1ms at 1mhz
  TACTL = TASSEL_2 + MC_1;             // SMCLK, upmode

  P1IFG  = 0x00;                //clear all interrupt flags
  P1DIR |= 0x01;                            // P1.0 as output for LED
  P1OUT &= ~0x01;                           // turn LED off

  _enable_interrupts();                     // global interrupt enable
  LPM0;

 while(1){
    P1IE &= ~0x01;          // disable interrupt
    P1DIR |= 0x02;          // trigger pin as output
    P1OUT |= 0x02;          // generate pulse
    __delay_cycles(10);             // for 10us
    P1OUT &= ~0x02;                 // stop pulse
    P1DIR &= ~0x04;         // make pin P1.2 input (ECHO)
     P1IFG = 0x00;                   // clear flag just in case anything happened before
    P1IE |= 0x04;           // enable interrupt on ECHO pin
    P1IES &= ~0x04;         // rising edge on ECHO pin
        __delay_cycles(30000);          // delay for 30ms (after this time echo times out if there is no object detected)
        distance = (int)duration/58;          // converting ECHO length into cm

        if(distance < 20 && distance != 0) P1OUT |= 0x01;  //turning LED on if distance is less than 20cm and if distance isn't 0.
        else P1OUT &= ~0x01;
 }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if(P1IFG & 0x04)  //is there interrupt pending?
        {
          if(!(P1IES & 0x04)) // is this the rising edge?
          {
            TACTL |= TACLR;   // clears timer A
            miliseconds = 0;
            P1IES |= 0x04;  //falling edge
          }
          else
          {
            duration = (long)miliseconds*1000 + (long)TAR;    //calculating ECHO length

          }
    P1IFG &= ~0x04;     //clear flag
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
   miliseconds++;
}
