#include <msp430.h> 

#define LED   BIT0
#define LED2  BIT6
#define BTN   BIT3

unsigned char toggle = 'F';
/**
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1DIR = LED | LED2;
    P1REN = BTN;
    P1OUT = BTN;

    do{
     P1OUT &= ~LED2;
     if((P1IN & BTN) == 0x00)
     {
         __delay_cycles(150000);
         toggle = 'T';
     }
     if(toggle != 'F')
     {
         P1OUT ^= LED;
         P1OUT |= LED2;
     }
    }while(1);
}
