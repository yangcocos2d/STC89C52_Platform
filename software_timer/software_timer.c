
#include "software_timer.h"
/*
soft_timer_stepper函数一般用在定时器的中断服务函数中
*/
void soft_timer_stepper(soft_timer* timer)
{
	if(timer->counter < timer->timeout)
	{
		timer->counter++;
	}
	
}
/*
soft_timer_check函数一般用在主循环中
*/
int soft_timer_check(soft_timer* timer)
{	
	if(timer->counter >= timer->timeout)
	{
		timer->counter = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}