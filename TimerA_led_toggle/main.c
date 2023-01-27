#include <msp430.h>
intrinsics.h

/* The LED connected to pin P1.0 is toggled using Timer_A every
* 50,000 SMCLK clock cycles through an interrupt service routine.
* The intrinsic function __BIS_SR_REGISTER is used to enter low power mode
* and enable maskable interrupts [35] */

void main(void){

    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    P1DIR |= 0x01; // P1.0 output

    CCTL0 = CCIE; // CCR0 interrupt enabled
    CCR0 = 50000;

    TACTL = TASSEL_2 + MC_2; // SMCLK, continuous mode
    _enable_interrupts();
    LPM0;
}

/* Timer A0 interrupt service routine */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR(void){
    P1OUT ^= 0x01; // Toggle P1.0
    CCR0  += 50000; // Add Offset to CCR0
}
