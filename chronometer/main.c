#include <msp430.h> 
#include "lcd.h"

#define START_STOP ((P1IFG & 0x01) == 0x01)
#define RESET      ((P1IFG & 0x02) == 0x02)

/*-----------------------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 * ---------------------------------------------------------------------------------------*/
volatile unsigned int start = 0;
volatile unsigned int sec_h = 0;
volatile unsigned int sec_l = 0;
volatile unsigned int min_h = 0;
volatile unsigned int min_l = 0;

char lcd_sec_h[1];
char lcd_sec_l[1];
char lcd_min_h[1];
char lcd_min_l[1];
char elapsedTime[5];
/*-----------------------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------------------------*/
void PinConfig(void);
void TimerSetup(void);
void WriteToLCD(void);

/*-----------------------------------------------------------------------------------------
 *  START OF APPLICATION
 * ---------------------------------------------------------------------------------------*/
void main(void)
{

    PinConfig();
    TimerSetup();
    Lcd_Init();

    _enable_interrupts();

    while(1){
        if(start){
            WriteToLCD();
            TA0CTL |= MC_1;
        }

        if(!start){
            WriteToLCD();
            LPM4;
       }
    }
}
/*-----------------------------------------------------------------------------------------
 *  INTERRUPT SERVICE ROUTINES
 * ---------------------------------------------------------------------------------------*/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){
    sec_l++;
    if(sec_l == 10){
        sec_l = 0;
        sec_h++;
        if(sec_h == 6){
            sec_h = 0;
            min_l++;
            if(min_l == 10){
                min_l = 0;
                min_h++;
            }
        }
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void){
    LPM4_EXIT;
    if(START_STOP){
        start ^= 1;
        P1IFG &= ~0x01;
    }
    if(RESET){
        sec_h = 0;
        sec_l = 0;
        min_h = 0;
        min_l = 0;
        start = 0;
        TACTL |= TACLR;
        P1IFG &= ~0x02;
    }
}

/*-----------------------------------------------------------------------------------------
 *  FUNCTION DEFINITIONS
 * ---------------------------------------------------------------------------------------*/
void PinConfig(void){
    P2SEL = 0x00;
    P1DIR = 0xFC;
    P2DIR = 0xFF;

    P1REN = 0x03;
    P1OUT = 0x03;
    P2OUT = 0x00;

    P1IE  = 0x03;
    P1IES = 0x03;
    P1IFG = 0x00;
}

void TimerSetup(void){
    WDTCTL  = WDTPW | WDTHOLD;   // stop watchdog timer
    BCSCTL3 |= LFXT1S_2;

    TACCTL0 = CCIE;
    TACTL   = MC_0|ID_3|TASSEL_1|TACLR;
    TACCR0  = 1499;
}

void WriteToLCD(void){
   IntToCharPtr(sec_h, lcd_sec_h, 10);
   IntToCharPtr(sec_l, lcd_sec_l, 10);
   IntToCharPtr(min_h, lcd_min_h, 10);
   IntToCharPtr(min_l, lcd_min_l, 10);

   elapsedTime[0] = lcd_min_h[0];
   elapsedTime[1] = lcd_min_l[0];
   elapsedTime[2] = ':';
   elapsedTime[3] = lcd_sec_h[0];
   elapsedTime[4] = lcd_sec_l[0];

   Lcd_GoTo(1, 1);
   Lcd_WrtStr(elapsedTime);
}

/*-----------------------------------------------------------------------------------------
 *  END OF PROGRAM
 * ---------------------------------------------------------------------------------------*/
