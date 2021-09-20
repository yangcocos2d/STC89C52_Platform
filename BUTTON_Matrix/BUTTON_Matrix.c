#include "BUTTON_Matrix.h"

typedef struct st_button_matrix_btn
{
		unsigned char button_pre;
		unsigned char button_cur;
}st_button_matrix_btn;

st_button_matrix_btn st_button_matrix_btn_data;


unsigned char button_matrix_scan_wait_free()
{
	unsigned char ret = 0;
	st_button_matrix_btn_data.button_pre = st_button_matrix_btn_data.button_cur;
	st_button_matrix_btn_data.button_cur = 	 button_matrix_scan();
	if( st_button_matrix_btn_data.button_cur == 0 
		&& st_button_matrix_btn_data.button_pre!=0)
		{
			ret = st_button_matrix_btn_data.button_pre;
			st_button_matrix_btn_data.button_pre = 0;

		}
	return ret;
}

void button_matrix_delayus(unsigned int us)
{
	unsigned int i = us/13;
	while(i--);

}

void button_matrix_delayms(unsigned int ms)
{
	unsigned int i =ms;
	for(i=ms;i>0;i--)
	{
			button_matrix_delayus(1000);
	}
}

unsigned char button_matrix_scan()
{
	unsigned char X_button,Y_button,code_button=0;
	
	KEY_PORT = 0x0F;
	button_matrix_delayms(10);
	Y_button = KEY_PORT; 
	Y_button&=0x0F;
	
	KEY_PORT = 0xF0;
	button_matrix_delayms(10);
	X_button = KEY_PORT ;
	X_button &=0xF0;
	
	switch(Y_button | X_button)
	{
		case 119:code_button = '7';break;
		case 123:code_button = '8';break;
		case 125:code_button = '9';break;
		case 126:code_button = ' ';break;
		case 183:code_button = '4';break;
		case 187:code_button = '5';break;
		case 189:code_button = '6';break;
		case 190:code_button = ' ';break;
		case 215:code_button = '1';break;
		case 219:code_button = '2';break;
		case 221:code_button = '3';break;
		case 222:code_button = ' ';break;
		case 231:code_button = ' ';break;
		case 235:code_button = ' ';break;
		case 237:code_button = ' ';break;
		case 238:code_button = ' ';break;
		default:break;
	}
	KEY_PORT = 0xFF;
	return code_button;//code_button;
}