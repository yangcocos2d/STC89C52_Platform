
#include "software_timer.h"
/*
soft_timer_stepper����һ�����ڶ�ʱ�����жϷ�������
*/
void soft_timer_stepper(soft_timer* timer)
{
	if(timer->counter < timer->timeout)
	{
		timer->counter++;
	}
	
}
/*
soft_timer_check����һ��������ѭ����
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