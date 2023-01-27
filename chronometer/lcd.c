/*
 * lcd.c
 *
 *  Created on: 2022. okt. 9.
 *      Author: NUC
 */
#include <msp430.h>
#include "lcd.h"

#define LCD_Data  P2OUT

void Lcd_Delay_ms(int time){
    while(time != 0){
        _delay_cycles(1000);
        time--;
    }
}

void Lcd_Change(void){
    P1OUT |= BIT7;
    P1OUT &= ~BIT7;
}

void Lcd_SendCmd(void){
    P1OUT &= ~BIT6;
}

void Lcd_SendChar(void){
    P1OUT |= BIT6;
}

void Lcd_Send(unsigned char data){
    LCD_Data = ((data & 0xF0) >> 4);
    Lcd_Change();
    LCD_Data = ((data & 0xF0));
    Lcd_Change();
}

void Lcd_WrtStr(const char* str){
    Lcd_SendChar();
    while(*str){
        Lcd_Send(*str++);
    }
}

void Lcd_WrtChar(char chr){
    Lcd_SendChar();
    Lcd_Send(chr);
}

void Lcd_GoTo(char x, char y){
    Lcd_SendCmd();
    if(x == 1) {
        Lcd_Send(0x80+((y-1)));
    }
    else {
        Lcd_Send(0xC0+((y-1)));
    }
}

void Lcd_Init(void){
    Lcd_SendCmd();
    Lcd_Delay_ms(40);
    Lcd_Send(0x30);
    Lcd_Delay_ms(1);
    Lcd_Send(0x28);
    Lcd_Delay_ms(1);
    Lcd_Send(0x28);
    Lcd_Delay_ms(1);
    Lcd_Send(0xC0);
    Lcd_Delay_ms(1);
    Lcd_Send(0x01);
    Lcd_Delay_ms(2);
    Lcd_Send(0x06);
}

char* IntToCharPtr(int val, char* res, int base){
    if(base < 2 || base > 36){
        *res = '\0';
        return res;
    }
    char* ptr = res;
    char* ptr2 = res, tmp_char;
    int tmp_val;

    do{
        tmp_val = val;
        val /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"\
                [35+(tmp_val-val*base)];
    } while(val);

    if(tmp_val < 0) *ptr++ = '-';
    *ptr-- = '\0';

    while(ptr2 < ptr){
        tmp_char = *ptr;
        *ptr-- = *ptr2;
        *ptr2++ = tmp_char;
    }
    return res;
}

char* FloatToCharPtr(float val, char* res, int base){
    if(base < 2 || base > 36){
        *res = '\0';
        return res;
    }
    char* ptr = res;
    char* ptr2 = res, tmp_char;
    float tmp_val;

    do{
        tmp_val = val;
        val /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"\
                [35+(tmp_val-val*base)];
    } while(val);

    if(tmp_val < 0) *ptr++ = '-';
    *ptr-- = '\0';

    while(ptr2 < ptr){
        tmp_char = *ptr;
        *ptr-- = *ptr2;
        *ptr2++ = tmp_char;
    }
    return res;
}


