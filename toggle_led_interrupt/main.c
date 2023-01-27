#include <msp430.h> 

#define RED_LED BIT0
#define BUTTON  BIT3

void stop_watchdog();
void red_led_setup();
void btn_interrupt_cfg();

void main(void)
{
	stop_watchdog();
	red_led_setup();
	btn_interrupt_cfg();

	_enable_interrupts();
	
	while(1); // wait for an interrupt
}

#pragma vector=PORT1_VECTOR
// define interrupt vector
__interrupt void PORT1_ISR(void)
{
    // Interrupt Service Routine
    // toggle red led
    P1OUT ^= 0x01;
    // clear interrupt flag
    P1IFG = 0x00;
}
void stop_watchdog(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    // stop watchdog timer
}

void red_led_setup(void)
{
    P1DIR = RED_LED;
    P1OUT = RED_LED;
}

void btn_interrupt_cfg(void)
{
    P1IE   = BUTTON; // enable interrupt from port 1
    P1IES  = BUTTON; // interrupt edge select from high to low
    P1IFG = 0x00;
}
