#ifndef _SOFTWARE_TIMER_H_
#define _SOFTWARE_TIMER_H_
#include <reg52.h>

typedef void (*p_soft_timer_fun)(void);
typedef struct soft_timer
{
	unsigned int counter;
	unsigned int timeout;
	p_soft_timer_fun fun;
}soft_timer;

void soft_timer_stepper(soft_timer* timer);
int soft_timer_check(soft_timer* timer);
#endif