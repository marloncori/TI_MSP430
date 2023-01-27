#include <msp430.h> 

#define redLed    BIT0
#define greenLed  BIT6
#define button    BIT3

/** The code given in Listing 10.17 is another example of the joint usage of the port and
 timer interrupts. Here the program counts how many times the button is pressed while the
 red and green LEDs are on separately. In addition, the system disables all the interrupts
 and goes to LPM4 after a certain time. This means the system is turned off using lowpower modes.
 *
 */

/*===========================================================================================
 *  GLOBAL VARIABLES
 * ==========================================================================================
 */
  volatile unsigned int count      = 0;
  volatile unsigned int redCount   = 0;
  volatile unsigned int greenCount = 0;
  volatile unsigned int state      = 0;
  volatile unsigned int done       = 0;
/*===========================================================================================
 *  START OF PROGRAM
 * ==========================================================================================
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P1DIR = redLed | greenLed;
	P1OUT = redLed;

	P1IE  = button;
	P1IES = button;
	P1IFG = 0x00;

  TACCTL0 = CCIE;   // enable interrupts on compare 0
   TACCR0 = 0xFFFF; // max counting limit -> 65535
    TACTL = TASSEL_2 | ID_3 | MC_3 | TACLR;
    /* use clock from SMCLk, divided by 8,
       up down mode, clear timer A */

   _enable_interrupts();

   while(1){
       if(done > 5){
          _disable_interrupts();
          LPM4;
       }
       else {
          LPM1;
       }
   }
}
/*===========================================================================================
 *   INTERRUPT SERVICE ROUTINES
 * ==========================================================================================
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){
    LPM1_EXIT;
    count++;
    if(count == 30){
        P1OUT ^= redLed | greenLed;
        state = !state;
        count = 0;
        done++;
    }
}
//--------------------------------
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    if(state == 0){
       redCount++;
    }
    else {
        greenCount++;
        P1IFG = 0x00;
    }
}

/*===========================================================================================
 *   END OF PROGRAM
 * ==========================================================================================
 */
