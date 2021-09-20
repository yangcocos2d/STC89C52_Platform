#include "exti_interrupt.h"

typedef void(*exti_callback)(void);
exti_callback p_exti0_callback = 0;
exti_callback p_exti1_callback = 0;


void exti_interrupt_init()
{
	EA = 1;
	EX0 = 1;
	IT0 = 1;
	
	IT1 = 1;
	EX1 = 1;
}


void exti0_interrupt_set_callback(void* pfun)
{
	IE0 = 0;
  p_exti0_callback = (exti_callback)pfun;
}

void exti1_interrupt_set_callback(void* pfun)
{
	IE0 = 0;
  p_exti1_callback = (exti_callback)pfun;
}


void exti_interrupt_ex0(void) interrupt 0
{	
	//EA = 0;
		if(p_exti0_callback!=0)
		{
			p_exti0_callback();
		}
	//EA = 1;
}

void exti_interrupt_ex1(void) interrupt 2
{	
	//EA = 0;
		if(p_exti1_callback!=0)
		{
			p_exti1_callback();
		}
	//EA = 1;
}