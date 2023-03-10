#include <msp430.h> 

volatile int temp[50];
volatile int diff[50];
volatile unsigned int i=0;
volatile unsigned int j=0;
volatile unsigned char hh = 0;
volatile unsigned char hl = 0;
volatile unsigned char th = 0;
volatile unsigned char tl = 0;
volatile unsigned char checksum = 0;
volatile unsigned char check = 0;
volatile unsigned char dataok = 0 ;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    __delay_cycles(2000000);
    P2DIR |= BIT4;
    P2OUT &= ~BIT4;
    __delay_cycles(20000);
    P2OUT |= BIT4;
    __delay_cycles(20);
    P2DIR &= ~BIT4;
    P2SEL |= BIT4;
    TA1CTL = TASSEL_2|MC_2;
    TA1CCTL2 = CAP | CCIE | CCIS_0 | CM_2 | SCS ;
    _enable_interrupts();
    while(1){
        if (i >= 40){
            for (j = 1; j <= 8; j++){
                if (diff[j] >= 110)
                    hh |= (0x01 << (8-j));
            }
            for (j = 9; j <= 16; j++){
                if (diff[j] >= 110)
                    hl |= (0x01 << (16-j));
            }
            for (j = 17; j <= 24; j++){
                if (diff[j] >= 110)
                    th |= (0x01 << (24-j));
            }
            for (j = 25; j <= 32; j++){
                if (diff[j] >= 110)
                    tl |= (0x01 << (32-j));
            }
            for (j = 33; j <= 40; j++){
                if (diff[j] >= 110)
                    checksum |= (0x01 << (40-j));
            }
            check = hh+hl+th+tl;
            if (check == checksum)
                dataok = 1;
            else
                dataok = 0;
        }

    }
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A1(void){
    temp[i] = TA1CCR2;
    i += 1;
    if (i>=2) diff[i-1]=temp[i-1]-temp[i-2];
    TA1CCTL2 &= ~CCIFG ;
}
