#include <msp430.h> 

/**
 * The string “Hello World” is transferred as a block to U1TXBUF using DMA0.
  UTXIFG1 WILL trigger DMA0. The rates is 9600 baud on UART1. The Watchdog
  timer triggers block transfer every 1000ms. Level sensitive trigger used
  for UTXIFG1 to prevent loss of initial edge sensitive triggers—UTXIFG1
  which is set at POR.

  ACLK = UCLK 32768Hz, MCLK = SMCLK = default DCO 1048576Hz
  Baud rate divider with 32768hz XTAL @9600 = 32768Hz/9600 = 3.41 (0003h)
 */

const char String1[13] = "\nHello World";

void main(void) {

    WDTCTL = WDT_ADLY_1000; // WDT 1000ms, ACLK, interval timer

    IE1 |= WDTIE; // Enable WDT interrupt
    P3SEL |= BIT6 + BIT7; // P3.6,7 = USART1 TXD/RXD
    //Configure USCIA1, UART~

    UCA1CTL1 = UCSSEL_1; // ACLK
    /* baud rate = 9600 */
    UCA1BR0 = 03;                       // 32768Hz 9600 32k/9600=3.41
    UCA1BR1 = 0x0;
    UCA1MCTL = UCBRS_3;                 // Modulation UCBRSx = 3

    /* Initialize USCI state machine */
    UCA1CTL1 &= ~UCSWRST;
                                        // Configure DMA0

    DMACTL0 = DMA0TSEL_10;              // UTXIFG1 trigger
    DMA0SA  = (int)String1;              // Source block address
    DMA0DA  = (int)&UCA1TXBUF;           // Destination single address
    DMA0SZ  = sizeof String1-1;          // Block size
    DMA0CTL = DMASRCINCR_3+DMASBDB+DMALEVEL;

                                        // Repeat, inc src, byte transfer
    __bis_SR_register(LPM3_bits + GIE); // Enter LPM3 w/ interrupts
    _NOP();                             // Reqd for debugger
}

#pragma vector = WDT_VECTOR // Trigger DMA block transfer
__interrupt void WDT_ISR(void)
{
    DMA0CTL |= DMAEN; // Enable
}
