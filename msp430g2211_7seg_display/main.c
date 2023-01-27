#include <msp430.h> 

//******************************************************************************
//  MSP430G2xx1 Demo - Controll 2 led 7 segments
//  MSP430G2211
//  Description:
//  ACLK = n/a, MCLK = SMCLK = default DCO
//  PHAN IT www.phanit.com
//  September 2011
//  Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//  link:
//      http://www.phanit.com/education/prjmsp430/G2211_2L7/G2211_1L7.html
//******************************************************************************

typedef struct {
  unsigned char index;
  unsigned char value;
} params;

params display[] = {
  {0,192}, {1,249}, {2,164}, {3,176}, {4,153}, {5,146},
  {6,130}, {7,248}, {8,128}, {9,144}, {10,255}
};

unsigned char To7Seg(unsigned char nVal,unsigned char nPt);
void Delay(unsigned int nLoop);

unsigned char mDigit[2];
unsigned char mDigNo;

void main(void)
{
    unsigned char i;

    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
    P2SEL = 0x00;                           // IO function is select
    P2DIR = 0xC0;                           // Port 2 output
    P2OUT = 0xC0;

    P1DIR |= 0xFF;                          // Port 1 output

    CCTL0 = CCIE;                           // CCR0 interrupt enabled
    CCR0 = 10000;                           // cycles to overload
    TACTL = TASSEL_2 + MC_1;                // SMCLK, upmode

    __enable_interrupt();                           // w/ interrupt

    mDigNo = 0;

    while(1)
    {
        for(i=0;i<100;i++)
        {
            mDigit[1] = To7Seg(i/10,0);
            mDigit[0] = To7Seg(i%10,0);
            Delay(50000);
        }   //for
    }
}
//******************************************************************************
// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  switch(mDigNo)
  {
    case 0:
        P2OUT = ~0x80;
        break;
    case 1:
        P2OUT = ~0x40;
        break;
  }
  P1OUT = mDigit[mDigNo];

  mDigNo++;
  if(mDigNo >1)
    mDigNo = 0;
}

//******************************************************************************
unsigned char To7Seg(unsigned char nVal,unsigned char nPt)
{
    unsigned char nRet, i;
    for(i=0;i<11;i++){
       if(nVal == display[i].index)
            nRet = display[i].value;
    }
    if(nPt)
        nRet = nRet & 127;

    return(nRet);
}
//******************************************************************************
void Delay(unsigned int nLoop)
{
    while(nLoop--);
}
//******************************************************************************
