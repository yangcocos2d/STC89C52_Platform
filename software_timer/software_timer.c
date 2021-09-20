/*
软件定时器是一种没有硬件定时器那么精准，但数量却几乎无限的"假定时器"。
一般在硬件定时器的服务函数中进行节拍递增，而在程序的主循环中判断时间是否已到；
如果时间已经到了，就调用该执行的函数。
*/

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