#include <msp430.h> 

const char msg[27] = "\nJesus Christ is the Lord!";

void main(void)
{
	WDTCTL = WDT_ADLY_1000;	// WDT 1000ms, ACLK interval timer
    IE1 |= WDTIE; // enable WDT Interrupt
    P3SEL |= BIT6 + BIT7; //P3.6, P3.7 = USART1 TXD, RXD
    //configure USCIA1, UART
    UCA1CTL1 = UCSSEL_1;    // ACLK
    //baud rate 9600
    UCA1BR0 = 03;       //32768Hz 9600 32k/6900=3.41
    UCA1BR1 = 0x0;
    UCA1MCTL = UCBRS_3;  //modulation UCBRSx = 3

    //initialize USCI state machine
    UCA1CTL1 &= ~UCSWRST;

    //configure DMA0
    DMACTL0 = DMA0TSEL_10; // UTXIFG1 trigger
    DMA0SA = (int)msg; // source block address
    DMA0DA = (int)&UCA1TXBUF;  // destination single address
    DMA0SZ = sizeof msg-1;     // block size
    DMA0CTL = DMASRCINCR_3 + DMASBDB + DMALEVEL;
                                //repeat, inc src, byte transfer
    __bis_SR_register(LPM3_bits + GIE); //enter LPM3 w/ interrupts
    _NOP();
}

#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    DMA0CTL |= DMAEN;
}
}
