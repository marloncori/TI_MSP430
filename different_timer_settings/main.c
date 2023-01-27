#include <msp430.h> 

#define LED          BIT0
#define LED2         BIT6
#define TOGGLE_LEDS (P1OUT ^= LED | LED2)

void setup_leds();
void stop_watchdog_timer();
void interrupt_enable_compare0();
void setup_timer();

void main(void)
{
    stop_watchdog_timer();
	setup_leds();
	interrupt_enable_compare0();
	/*
	 * TACTL = TASSEL_2 | ID_3 | MC_2 | TACLR;
	 * use clock from SMCLK, divide clock by 8
	 * up mode, clear timer A
	 *
	 * OR
	 * TACTL = TASSEL_2 | ID_0 | MC_2 | TACLR;
	 * the effect of the frequency divider
	 * TACCR0 = 19999; // upper limit for count
	 *
	 * TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR;
	 * */
	setup_timer();
	
	_enable_interrupts();
	LPM1;
}

void stop_watchdog_timer(void){
    WDTCTL = WDTPW | WDTHOLD;
}

void setup_leds(void)
{
    P1DIR  = LED | LED2; // output
    P1OUT  = LED2; //LED low, LED2 high
}

void interrupt_enable_compare0(void)
{
    TACCTL0 = CCIE;
}

void setup_timer(void)
{
    TACCR0 = 0xFFFF; // Upper limit for count
    //TACCR0 = 19999;
    TACTL  = TASSEL_2 | ID_3 | MC_2 | TACLR;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void)
{
    TOGGLE_LEDS;
}
