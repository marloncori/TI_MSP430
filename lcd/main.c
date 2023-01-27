/* chronometer with lcd project */

#include <msp430.h>
#include "lcd.h"

#define START_STOP ((P1IFG & 0x01) == 0x01)
#define RESET      ((P1IFG & 0x02) == 0x02)

int start = 0;
int second_h = 0;
int second_l = 0;
int minute_h = 0;
int minute_l = 0;

char lcd_sec_h[1];
char lcd_sec_l[1];
char lcd_min_h[1];
char lcd_min_l[1];

char lcd[5];

void pin_config(void);
void timer_config(void);
void write_to_lcd(void);

void main(void)
{
    pin_config();
    timer_config();

    lcd_init();
    _enable_interrupts();

    while(1)
    {
        if(start == 1)
        {
           write_to_lcd();
           TA0CTL |= MC_1;
        }
        if(start == 0)
        {
           write_to_lcd();
           LPM4;
        }
    }
}

void pin_config(void)
{
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

void timer_config(void)
{
    WDTCTL  = WDTPW | WDTHOLD;
    BCSCTL3 = LFXT1S_2;

    TACCTL0 = CCIE;
    TACTL   = MC_0 | ID_3 | TASSEL_1 | TACLR;
    TACCR0  = 1499;
}

void write_to_lcd(void)
{
    itoa(second_h, lcd_sec_h, 10);
    itoa(second_l, lcd_sec_l, 10);
    itoa(minute_h, lcd_min_h, 10);
    itoa(minute_l, lcd_min_l, 10);

    lcd[0] = lcd_min_h[0];
    lcd[1] = lcd_min_l[0];
    lcd[2] = ':';
    lcd[3] = lcd_sec_h[0];
    lcd[4] = lcd_sec_l[0];

    lcd_goto(1, 1);
    lcd_write_str(lcd);
}

#pragma vector = TIMER0_A0_VECTOR;
__interrupt void int_timer(void)
{
    second_l++;
    if(second_l == 10)
    {
        second_l = 0;
        second_h++;

       if(second_h == 6)
       {
           second_h = 0;
           minute_l++;

          if(minute_l== 10)
          {
             minute_l = 0;
             minute_h++;
          }
       }
    }
}

#pragma vector = PORT1_VECTOR;
__interrupt void int_button(void)
{
    LPM4_EXIT;
    if(START_STOP)
    {
        start ^= 1;
        P1IFG &= ~0x01;
    }
    if(RESET)
    {
        second_h = 0;
        second_l = 0;
        minute_h = 0;
        minute_l = 0;
        start    = 0;
        TACTL   |= TACLR;
        P1IFG   &= ~0x02;
    }
}
