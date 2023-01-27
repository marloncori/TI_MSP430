#include <msp430.h> 

int data = 0;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ; // adjust the clock
    DCOCTL= CALDCO_1MHZ;

    // adjust pins
    P1DIR |= BIT6;
    P1OUT = 0x00;

    P1SEL  = BIT1 | BIT2;
    P1SEL2 = BIT1 | BIT2;

    P1IE  |= 0x08;
    P1IES |= 0x08;
    P1IFG  = 0x00;

    // setup the uart mode
    UCA0CTL1 |= UCSSEL_2;

    // use SMCLK
    UCA0BR0 = 104;
    // low bit of UCBRx is 104
    UCA0BR1 = 0;
    // high bit of UCBRx is 0
    UCA0MCTL = UCBRS_1;
    //second modulation stage select is 1
    //baud rate = 9600
    UCA0STAT |= UCLISTEN;
    // enable internal loopback
    UCA0CTL1 &= ~UCSWRST;
    //clear SW reset, resume operation
    IE2 |= UCA0RXIE | UCA0TXIE;
    //enable USCI_A0 RX TX interrupt

    _enable_interrupts();

    LPM4;
}

//USCI A transmitter interrupt
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIA0TX_ISR(void)
{
  UCA0TXBUF = data;
  // load the TX buffer with integer value
}

//USCI A receiver interrupt
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
  P1OUT = UCA0RXBUF;
  // write received data to P1OUT
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_IRQ(void)
{
    data ^= 0x40;
    // toggle data value
    P1IFG = 0x00;
    //clear interrupt flags
}
