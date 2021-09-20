
#include "timer.h"

typedef char (*timer0_callback)(char para);

timer0_callback p_timer0_callback = 0;

void Timer0_Init()
{
 
	TL0 = TIMER_RELOAD%256;   
	TH0 = TIMER_RELOAD/256;
	TMOD |=0x01;
	ET0 = 1;
	EA=1;
	TR0 = 1;
	
}

void Timer0_interupt_enable()
{
		ET0 = 1;
	
}
void Timer0_interupt_disable()
{
		ET0 = 0;
}

void Timer0_set_callback(void* pfun)
{
    p_timer0_callback = (timer0_callback)pfun;
}


void timer0_irq() interrupt 1
{
	EA = 0;
	TL0 = TIMER_RELOAD%256;   
	TH0 = TIMER_RELOAD/256;
	
	if(p_timer0_callback != 0)
	{
			SP = p_timer0_callback(SP); //使用多线程操作系统
	}
	EA = 1;
}