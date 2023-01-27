#include <msp430.h> 

/*
*  #---------------------------------------------------------------#
*  # Check this very good tutorial:               #
*  #  http://www.ocfreaks.com/msp430-timer-programming-tutorial/   #
*  #---------------------------------------------------------------#
 */

void main( void )
{
  WDTCTL    = WDTPW | WDTHOLD;   // Stop watchdog timer

  BCSCTL1   = CALBC1_1MHZ;       // Set DCO range
  DCOCTL    = CALDCO_1MHZ;       // Set DCO step and modulation

  P1DIR    |=  0x04;             // Set P1.2 to output-direction
  P1SEL    |=  0x04;             // Set selection register 1 for timer-function
  P1SEL2   &= ~0x04;             // Clear selection register 2 for timer-function (not needed, as it is 0 after startup)

  TA0CCTL1  = OUTMOD_7;          // Reset/set
  TA0CCR0   = 20000;             // Period
  TA0CCR1   = 1500;              // Duty-cycle
  TA0CTL    = (TASSEL_2 | MC_1); // SMCLK, timer in up-mode

  while( 1 )                     // Endless-loop (main-program)
  {
                                 // Do nothing, hardware-based PWM
  }
}
