#include <msp430.h>

/* source:
*
* http://www.ocfreaks.com/msp430-timer-programming-tutorial/
*
* For a 1 MHz clock, input divider 8, up-down-mode,
* TACCR0 can be set to 62499 to obtain 0.5 to up limit
* plus another 0.5 to lower boundary, generating a 1 sec delay.
*--------------------------------------------------------------------
* If I used a counter to get track of how many timer the up
* and down overflow have occured, all I have to do is incrementing it
* in the ISR and check it is value still in the ISR (OVCount == 2),
* for a two second delay, or (OVCount == 5) for five seconds and so on.
*---------------------------------------------------------------------
* For a 6 MHz clock and ID_3 (divider 8), I obtain TACCR0 = 46874
* for a 0.0625 overflow time, what means 0.0625 * 16 = 1 second,
* so all I have to do is incrementing a counter in the ISR and check
* whether it is value has reached 16 (OVCount == 16) for a 1 sec delay
* or 32 for a 2 sec or 64 for a 4 second delay and so on.
* ---------------------------------------------------------------------
*/

void initTimer_A(void);
void delayMS(int msecs);

unsigned int OFCount;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; //Stop watchdog timer
    P1DIR |= BIT0; //Configure P1.0 as Output

    //Set MCLK = SMCLK = 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    initTimer_A();
    _enable_interrupt();

    while(1)
    {
        P1OUT |= BIT0; //Drive P1.0 HIGH - LED1 ON
        delayMS(500); //Wait 0.5 Secs

        P1OUT &= ~BIT0; //Drive P1.0 LOW - LED1 OFF
        delayMS(500); //Wait 0.5 Secs
    }
}

void initTimer_A(void)
{
    //Timer0_A3 Configuration
    TACCR0 = 0; //Initially, Stop the Timer
    TACCTL0 |= CCIE; //Enable interrupt for CCR0.
    TACTL = TASSEL_2 + ID_0 + MC_1; //Select SMCLK, SMCLK/1, Up Mode
}

void delayMS(int msecs)
{
    int i;
    OFCount = 0; //Reset Over-Flow counter
    TACCR0 = 1000-1; //Start Timer, Compare value for Up Mode to get 1ms delay per loop
    //Total count = TACCR0 + 1. Hence we need to subtract 1.

    while(i<=msecs){}

    TACCR0 = 0; //Stop Timer
}

//Timer ISR
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    OFCount++; //Increment Over-Flow Counter
}
