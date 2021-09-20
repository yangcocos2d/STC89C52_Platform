#ifndef _BUTTON_H_
#define _BUTTON_H_
#include <reg52.h>

sbit esc_button = P1^0;
sbit up_button  = P1^1;
sbit down_button  = P1^2;
sbit enter_button  = P1^3;


#define BUTTON_ESC 			0
#define BUTTON_UP 			1
#define BUTTON_DOWN 		2
#define BUTTON_ENTER 		3


#define BUTTON_PRESS		0
#define BUTTON_FREE		  	1
#define BUTTON_LONG_PRESS	2

int button_get_status(int btn_no,unsigned int btn_press_cnt);

#endif