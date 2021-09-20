#ifndef _LCD1602_H_
#define _LCD1602_H_

#include <reg52.h>
sbit LCD_E = P2^7;
sbit LCD_RW = P2^6;
sbit LCD_RS = P2^5;
#define LCD_DATA  P1

#define LCD1602_X_LIMIT   16

void LCD_Init();
void LCD_Puts(char x,char y,char *str);
void LCD_Clear(int line_y);
void LCD_delay_10us() ;
void LCD_PutcOnxy(char x,char y,char Data);
#endif