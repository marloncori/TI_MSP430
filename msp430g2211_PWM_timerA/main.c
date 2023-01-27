#include <msp430.h>

//  D. Dang
//  Texas Instruments, Inc
//  October 2010
//  Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************

void Delay(unsigned int nLoop);

void main(void)
{
    unsigned int n;

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |= 0x0C;                            // P1.2 and P1.3 output
  P1SEL |= 0x0C;                            // P1.2 and P1.3 TA1/2 options
  CCR0 = 512-1;                             // PWM Period
  CCTL1 = OUTMOD_7;                         // CCR1 reset/set
  CCR1 = 384;                               // CCR1 PWM duty cycle
  TACTL = TASSEL_2 + MC_1;                  // SMCLK, up mode

  //_BIS_SR(CPUOFF);                          // Enter LPM0

  while(1)
  {
    for(n=0;n<512;n++)
    {
        CCR1 = n;
        Delay(1000);
    }
  }
}

//******************************************************************************
void Delay(unsigned int nLoop)
{
    while(nLoop--);
}
//******************************************************************************
