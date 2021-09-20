
#include <stdio.h>
#include <reg52.h>
#include "timer.h"
#include "software_timer.h"
#include "lcd1602.h"
#include "mystring.h"
#include "UART.h"
#include "KeyBoard.h"


#define KEY_NUM		22


KEY_BLOCK key_num[KEY_NUM] = {
{0,0,'('},
{1,0,')'},
{2,0,'<'},
{3,0,'C'},

{0,1,'7'},
{1,1,'8'},
{2,1,'9'},
{3,1,'+'},

{0,2,'4'},
{1,2,'5'},
{2,2,'6'},
{3,2,'-'},

{0,3,'1'},
{1,3,'2'},
{2,3,'3'},
{3,3,'*'},

{0,4,'0'},
{1,4,'.'},
{2,4,'='},
{3,4,0xFD},

{4,0,'L'},
{5,0,'R'},
};

typedef struct STR_INPUT
{
	char str_input[100];
	char str_result[100];
	char str_input_tail;
	char cursor_pos;			

	int dis_window_left;
	int dis_window_right;
}STR_INPUT;

void STR_window_set(STR_INPUT* str)
{
	
	str->dis_window_right = str->cursor_pos;
	
	str->dis_window_left = str->dis_window_right - ( LCD1602_X_LIMIT - 1);
	if(str->dis_window_left < 0)
	{
		str->dis_window_left = 0;
		str->dis_window_right = LCD1602_X_LIMIT-1;
	}
 
}

void STR_fold(STR_INPUT* str)
{
	str->cursor_pos = str->str_input_tail;
	STR_window_set(str);
}

void STR_input_char(STR_INPUT* str,char ch)
{
	str->str_input[str->str_input_tail++] = ch;

	STR_fold(str);
}	

void STR_del_char(STR_INPUT* str)
{
	if(str->str_input_tail > 0)
	{
		str->str_input[--str->str_input_tail] = ' ';
		STR_fold(str);
	}
}

void STR_left_move(STR_INPUT* str)
{
	if(str->cursor_pos > 0)
	{
		str->cursor_pos--;
		STR_window_set(str);
	}
}

void STR_right_move(STR_INPUT* str)
{
	if(str->cursor_pos < str->str_input_tail)
	{
		str->cursor_pos++;
		STR_window_set(str);
	}
}

void STR_init(STR_INPUT* str)
{
	int i = 0;
	str->str_input_tail = 0;
	str->dis_window_left = str->dis_window_right = 0;
	str->cursor_pos = 0;
	for(i=0;i<100;i++)
	{
		str->str_input[i] = '\0';
	}
	for(i=0;i<(LCD1602_X_LIMIT-1);i++)
	{
		str->str_result[i] = ' ';
	}
	str->str_result[LCD1602_X_LIMIT-1] = '0';
	str->str_result[LCD1602_X_LIMIT] = '\0';
}

STR_INPUT str;
void keyboard_scan_timer_Handle()
{
	int i;
	unsigned char Key_Code = 0;
	
	for(i=0;i<KEY_NUM;i++)
	{		
			Key_Code = KeyBoard_is_loosen(&key_num[i]);
			if(Key_Code >='0' && Key_Code <= '9' ||
				Key_Code == '.' ||
				Key_Code == '=' ||
				Key_Code == '+' ||
				Key_Code == '-' ||
				Key_Code == '*' ||
				Key_Code == 0xFD||
				Key_Code == '(' ||
				Key_Code == ')' )
			{
				STR_input_char(&str,Key_Code);
			}
			if(Key_Code == '<')
			{
				STR_del_char(&str);
			}
			if(Key_Code == 'C')
			{
				STR_init(&str);	
			}
			if(Key_Code == 'L')
			{
				 STR_left_move(&str);
			}
			if(Key_Code == 'R')
			{
				 STR_right_move(&str);
			}
			if(Key_Code == '=')
			{
				 
			}
	}
}

soft_timer keyboard_scan_timer = {0,10,keyboard_scan_timer_Handle};

char LCD_Disbuffer[2][16];

void lcd_update_timer_Handle()
{
	static int input_flash = 0;
	int dis_cursor_pos = str.cursor_pos - str.dis_window_left; //相对于真实LCD的光标
	char dis_flash = str.str_input[str.cursor_pos]; //闪烁字符
	
	str_copy(LCD_Disbuffer[0],"                ");
	str_copy(LCD_Disbuffer[1],"                ");

	Timer0_interupt_disable();
	if(str.str_input_tail == 0)
	{
		LCD_Clear(0);
	}
	else
	{
		str_copy(LCD_Disbuffer[0],&(str.str_input[str.dis_window_left]));
		
	}
	if(input_flash == 1)
	{
		LCD_Disbuffer[0][dis_cursor_pos] = '_';
	}
	else
	{
		if(dis_flash == '\0') 
		{
			dis_flash = ' ';
		}
		LCD_Disbuffer[0][dis_cursor_pos] = dis_flash;

	}
	if(str.dis_window_left > 0)
	{
		LCD_Disbuffer[0][0] = 0x7F;//左箭头
	}
	if(str.dis_window_right < str.str_input_tail)
	{
		LCD_Disbuffer[0][LCD1602_X_LIMIT-1] = 0x7E;//右箭头
	}
	
	
	LCD_Puts(0,0,LCD_Disbuffer[0]);
	LCD_Puts(0,1,str.str_result);
	input_flash = !input_flash;
	Timer0_interupt_enable();

}
soft_timer lcd_update_timer = {0,100,lcd_update_timer_Handle};

char Timer0_Handle()
{

	soft_timer_stepper(&keyboard_scan_timer);
	soft_timer_stepper(&lcd_update_timer);
}

void main()
{
 
	Uart_init();
	LCD_Init();
	Timer0_set_callback(Timer0_Handle);
	Timer0_Init();
	STR_init(&str);
	while(1)
	{
			if(soft_timer_check(&keyboard_scan_timer) == 1)
			{
				keyboard_scan_timer.fun();
			}
			if(soft_timer_check(&lcd_update_timer) == 1)
			{
				lcd_update_timer.fun();
			}

	}
}

