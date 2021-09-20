#ifndef _TIMER_H_
#define _TIMER_H_
#include <reg52.h>

#define TIMER_RELOAD    (65536 - 10000)
#define TIMER_INT()			TF0 = 1

void Timer0_Init();
void Timer0_interupt_enable();
void Timer0_interupt_disable();
void Timer0_set_callback(void* pfun);
void Timer0_int();
#endif