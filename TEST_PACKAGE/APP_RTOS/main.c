#include <reg52.h>
#include "UART.h"
#include "timer.h"
#include "Thread.h"
#include "exti_interrupt.h"

//线程A
char  idata thread_a_stack[30];
thread_block  thread_a;
void thread_a_fun()
{
	static int test_data = 0;
	while(1)
	{	
		if(test_data >= 10)
		{
			test_data = 0;
		}
		putchar(test_data + '0');
		test_data++;
	}

}

//线程B
char idata thread_b_stack[30];
thread_block   thread_b;
void thread_b_fun()
{
	static int test_data = 'a';
	while(1)
	{
		if(test_data > 'z')
		{
			test_data = 'a';
		}
		putchar('b');
		test_data++;
	}

}

//线程C
char idata thread_c_stack[30];
thread_block   thread_c;

void thread_c_fun()
{
	static int test_data = 'A';
	while(1)
	{
		if(test_data > 'Z')
		{
			test_data = 'A';
		}
		
		putchar(test_data);
		test_data++;
	}
}

void EXTI0_int_Handler()
{
	putchar('\n');
	thread_set_next_thread(&thread_b);
	TIMER_INT();
}

void  main()
{
 	Uart_init();
	putchar('$');
	thread_create(&thread_a,thread_a_fun,thread_a_stack,"thread_a");
	thread_create(&thread_b,thread_b_fun,thread_b_stack,"thread_b");
	thread_create(&thread_c,thread_c_fun,thread_c_stack,"thread_c");

	Timer0_set_callback(thread_change);
	Timer0_Init();
	exti0_interrupt_set_callback(EXTI0_int_Handler);
	exti_interrupt_init();
	
	thread_start();
	while(1)
	{
		  
	}

} 