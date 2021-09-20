#ifndef _EXTI_INTERRUPT_H_
#define _EXTI_INTERRUPT_H_
#include <reg52.h>

void exti_interrupt_init();
void exti0_interrupt_set_callback(void* pfun);
void exti1_interrupt_set_callback(void* pfun);

#endif