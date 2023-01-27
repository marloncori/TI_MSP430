/*
 * lcd.h
 *
 *  Created on: 2022. jún. 17.
 *  Author: Marlon Couto Ribeiro
 */

#ifndef LCD_H_
#define LCD_H_

#include <msp430.h>

#define LCD_Data     P2OUT
#define LCD_Change() ((P1OUT |= BIT7), (P1OUT &= ~BIT7))
#define Send_Cmd     (P1OUT &= ~BIT6)
#define Send_Char    (P1OUT |= BIT6)

void delay_ms(int cnt)
{
    while(cnt != 0)
    {
        _delay_cycles(1000);
        cnt--;
    }
}

void lcd_send(unsigned char data)
{
    LCD_Data = ((data & 0xF0) >> 4);
    LCD_Change();
    LCD_Data = ((data & 0x0F));
    LCD_Change();
}

void lcd_write_str(const char *data)
{
    Send_Char;
    while(*data)
    {
        lcd_send(*data);
    }
}

void lcd_write_char(char data)
{
    Send_Char;
    lcd_send(data);
}

void lcd_goto(char x, char y)
{
    Send_Cmd;
    if(x == 1)
    {
        lcd_send(0x80 + ((y-1)));
    }
    else
    {
        lcd_send(0xC0 + ((y-1)));
    }
}

void lcd_init(void)
{
    Send_Cmd;
    delay_ms(40);
    lcd_send(0x30);
    delay_ms(1);
    lcd_send(0x28);
    delay_ms(1);
    lcd_send(0x28);
    delay_ms(1);
    lcd_send(0x0C);
    delay_ms(1);
    lcd_send(0x01);
    delay_ms(2);
    lcd_send(0x06);
}

char* itoa(int value, char* result, int base)
{
    if(base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do{
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"\
       [35 + (tmp_value * base)];
    } while(value);

    if(tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';

    while(ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr--   = *ptr1;
        *ptr1++  = tmp_char;
    }
    return result;
}

#endif /* LCD_H_ */
