#include "button.h"
/*按纽定义*/

typedef int(*p_fun_get_button_status)(void);
typedef struct button
{
	p_fun_get_button_status get_status;	 
	int button_press_counter;						 
}button;

int get_esc_button()
{
	esc_button = 1;
	return esc_button;
}

int get_up_button()
{
	up_button = 1;
	return up_button;
}

int get_down_button()
{
	down_button = 1;
	return down_button;
}

int get_enter_button()
{
	enter_button =1;
	return enter_button;
}

button btn_array[] =
{
	{get_esc_button,0},
	{get_up_button,0},
	{get_down_button,0},
	{get_enter_button,0}
};

/*按纽API
btn_no:按钮编号
btn_press_cnt:检查持续按压的时间节拍，具体时长根据函数被调用时的上下文而定。
返回值:存在没按下、按下、和持续按下。持续按下是指已经被按了btn_press_cnt的时间。
*/
int button_get_status(int btn_no,unsigned int btn_press_cnt)
{	
	button* btn = &btn_array[btn_no];
	int btn_status = BUTTON_FREE;
	btn_status = btn->get_status();
	if(btn_status == BUTTON_PRESS)
	{
		if(btn->button_press_counter<65535)
		{
			btn->button_press_counter++;
		}
	}
	else
	{
		btn->button_press_counter = 0;
	}
	
	if(btn->button_press_counter >= btn_press_cnt)
	{
		btn_status = BUTTON_LONG_PRESS;
		btn->button_press_counter = 0;
	}
	
	return btn_status;
}

