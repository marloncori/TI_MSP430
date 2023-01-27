/*
 * lcd.h
 *
 *  Created on: 2022. okt. 9.
 *      Author: NUC
 */

#ifndef LCD_H_
#define LCD_H_


void Lcd_Delay_ms(int time);
void Lcd_Change(void);
void Lcd_SendCmd(void);
void Lcd_SendChar(void);
void Lcd_Send(unsigned char data);
void Lcd_WrtStr(const char* str);
void Lcd_WrtChar(char chr);
void Lcd_GoTo(char x, char y);
void Lcd_Init(void);
char* IntToCharPtr(int val, char* res, int base);
char* FloatToCharPtr(float val, char* res, float base);


#endif /* LCD_H_ */
