
#include "LCD1602.h"
#include <intrins.h>

void LCD_delayus(unsigned int us)
{
	 unsigned int i = us /13;
	 while(i--);
}
 
int idata lcd_delay_us_set = 1000;
void LCD_delay_10us()   
{
	if(lcd_delay_us_set <= 10)
	{
	
	}
	else
	{
		LCD_delayus(lcd_delay_us_set);
	}
}

unsigned char LCD_ReadStatus()
{
	unsigned char Data = 0;
	LCD_RS = 0;
	LCD_RW = 1;
	LCD_DATA = 0xFF;
	LCD_E = 1;
	LCD_delay_10us();
	Data = LCD_DATA;
	LCD_E = 0;
	LCD_delay_10us(); 
	return Data;
}

unsigned char LCD_ReadData()
{
	unsigned char Data = 0;

	LCD_RS = 1;
	LCD_RW = 1;
	LCD_DATA = 0xFF;
	LCD_E = 1;
	LCD_delay_10us();
	Data = LCD_DATA;
	LCD_E = 0;
	LCD_delay_10us();
	return Data;
}

void LCD_WriteCmd(unsigned char Cmd)
{
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_DATA = Cmd;
	LCD_E = 1;
	LCD_delay_10us();
	LCD_E = 0;
	LCD_delay_10us();
}

void LCD_WriteData(unsigned char Data)
{ 
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_DATA = Data;
	LCD_E = 1;	
	LCD_delay_10us();
	LCD_E = 0;
	LCD_delay_10us();
}

#define LCD_CMD_CLEAR 		0x01
#define LCD_CMD_OFF				0x08
#define LCD_CMD_ON 				0x0C
#define LCD_CMD_SET 			0x38  

void LCD_Init()
{
	lcd_delay_us_set = 1000;
	LCD_WriteCmd(LCD_CMD_OFF);
	LCD_WriteCmd(LCD_CMD_ON);
	LCD_WriteCmd(LCD_CMD_CLEAR);
	LCD_WriteCmd(LCD_CMD_SET); 
	lcd_delay_us_set = 0;
}

/*
��Ļ��ӡ������ϵ�涨����:
o-------------------->x  
|
|
|
v
y
 
*/
void LCD_PutcOnxy(char x,char y,char Data)
{

	LCD_WriteCmd( x   +0x80 + y*0x40 );
	LCD_WriteData(Data);
}

void LCD_Puts(char x,char y,char *str)
{
	int i = 0;
	LCD_WriteCmd(x + 0x80 + y*0x40);
	while(str[i]!='\0' && i<=(LCD1602_X_LIMIT - 1))
	{	 
		LCD_WriteData(str[i]);
		i++;
	}
	
}

void LCD_Clear(int line_y)
{
	LCD_Puts(0,line_y,"                ");
}