#include <msp430.h> 

//******************************************************************************
// MSP430G2xx1 Demo - Software Toggle P1
//
// Description; Toggle P1 inside of a software loop.
// ACLK = n/a, MCLK = SMCLK = default DCO
//
// MSP430G2211

// PHAN IT
// September 2011
// Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
// link
//   http://www.phanit.com/education/prjmsp430/G2211_leds/G2211_leds.html
//******************************************************************************

void Delay(unsigned int nLoop);

void main(void)
{
  unsigned int j;
  unsigned char nB;

  WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
  P1DIR |= 0xFF; // Set P1.0 to output direction

  nB = 0x01;
  while(1)
  {
    for(j=0;j<8;j++)
    {
      P1OUT = nB;
      Delay(50000);
      if(nB==0x80)
        nB = 0x01;
      else
        nB = nB << 1;
     } //for
  } //while
}

//******************************************************************************
void Delay(unsigned int nLoop)
{
  while(nLoop--);
}
//******************************************************************************
