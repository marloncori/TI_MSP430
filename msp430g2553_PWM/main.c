#include <msp430.h> 

/*
* #---------------------------------------------------------------#
* # Check also this very good tutorial:                           #
* #  http://www.ocfreaks.com/msp430-timer-programming-tutorial/   #
* #---------------------------------------------------------------#
*
*/

void ConfigWatchDog(void);
void ConfigClock(void);
void ConfigPin(void);
void ConfigPWM(void);

void main( void )
{
  ConfigWatchDog();
  ConfigClock();
  ConfigPin();
  ConfigPWM();

  while( 1 )                     // Endless-loop (main-program)
  {
                                 // Do nothing, hardware-based PWM
  }
}

void ConfigWatchDog(void){
  WDTCTL    = WDTPW | WDTHOLD;   // Stop watchdog timer
}

void ConfigClock(void){
  BCSCTL1   = CALBC1_1MHZ;       // Set DCO range
  DCOCTL    = CALDCO_1MHZ;       // Set DCO step and modulation
}

void ConfigPin(void){
  P2DIR    |=  0x02;             // Set P2.1 to output-direction
  P2SEL    |=  0x02;             // Set selection register 1 for timer-function
  P2SEL2   &= ~0x02;             // Clear selection register 2 for timer-function (not needed, as it is 0 after startup)
}

void ConfigPWM(void){
  TA1CCTL1  = OUTMOD_7;          // Reset/set
  TA1CCR0   = 20000;             // Period
  TA1CCR1   = 1500;              // Duty-cycle
  TA1CTL    = (TASSEL_2 | MC_1); // SMCLK, timer in up-mode
}

