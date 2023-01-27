#include <msp430.h> 

/*
 * In this section, we provide sample C and assembly codes for PWM generation. We benefit
from the capture/compare block of the TA module in generating the PWM. In Listing
11.10, the period and the duty cycle of the generated PWM can be adjusted by two
variables. The output is fed to the green LED of the MSP430 LaunchPad. Therefore, the
PWM signal can be observed by the dimness of the LED.
 */

void main(void)
{
    // stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
	// period of the PWM
	int period = 0x0FFF;
	// duty cycle, max value 1
	float D = .8;

	P1DIR |= BIT6;
	P1SEL |= BIT6;

	// PWM period
	TACCR0  = period - 1;
	// CCR1 PWM duty cycle
	TACCR1  = period * D;
	// CCR1 reset/set
	TACCTL1 = OUTMOD_7;
	// SMCLK, up mode
	TACTL   = TASSEL_2 | MC_1;
	
	LPM1;
}
