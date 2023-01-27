#include <msp430.h> 

//******************************************************************************
//  MSP430G2xx1 Demo - Poll P2 With Software with Internal Pull-up
//
//  Description: Poll P2.7 in a loop, if hi P2.6 is set, if low, P2.6 reset.
//  Internal pullup enabled on P2.7.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//               MSP430G2211
//            -----------------
//        /|\|              XIN|-
//         | |                 |
//         --|RST          XOUT|-
//     /|\   |      R          |
//      --o--| P2.7-o      P2.6|-->LED
//     \|/
//
//  PHAN IT
//  September 2011
//  Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************


#include  <msp430g2211.h>

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P2SEL = 0x00;                             // IO function is select
  P2DIR = 0x40;                             // P2.6 output, else input
  P2OUT = 0x80;                             // P2.7 set, else reset
  P2REN |= 0x80;                            // P2.7 pullup

  while (1)                                 // Test P2.7
  {
    if (0x80 & P2IN) P2OUT |= 0x40;         // if P2.7 set, set P2.6
    else P2OUT &= ~0x40;                    // else reset
  }
}

