#include <msp430.h> 

#define LED1           BIT0
#define LED2           BIT6
#define TOGGLE_LEDS() (P1OUT ^= LED1 | LED2)
/**
 * main.c
 */
void main(void)
{
    BCSCTL2 |= DIVS_3;
    WDTCTL   = WDT_MDLY_32;
    IE1     |= WDTIE;

    P1DIR    = LED1 | LED2;
    P1OUT    = LED1;

    _enable_interrupts();

    LPM1;
}

#pragma vector=WDT_VECTOR
__interrupt void WDT(void)
{
    TOGGLE_LEDS();
}
