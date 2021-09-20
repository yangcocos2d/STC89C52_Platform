#include "LCD1602.h"
#include "UART.h"
#include "button.h"
#include "timer.h"
#include "pwm.h"
#include "software_timer.h"
#include "mystring.h"
#include "iic.h"
#include "pcf8591.h"

sbit FET_A = P2^0;
sbit FET_B = P2^1;
sbit Boost_Ctrl = P2^2;
 
#define SPWM_OUT_LIST_LEN 10


 
int pwm_out = 0;
int idata pwm_out_list[SPWM_OUT_LIST_LEN ] = 	 {0,1,0,1,1,1,1,0,1,0};
int idata pwm_index = 0;
pwm idata pwm_boost = {50,1000,0};

void fetctrl_gen()  //1ms 
{
 
	 pwm_index++;
	 if(pwm_index >= 2*SPWM_OUT_LIST_LEN)
	 {
		 pwm_index = 0;
	 }
	 
	 pwm_out =  pwm_out_list[pwm_index%SPWM_OUT_LIST_LEN];
	 
	 if(pwm_out == 0)
	 {
			FET_A= 1;
		  FET_B =1;
	 }
	 else
	 {
			if(pwm_index >= SPWM_OUT_LIST_LEN)
		 {
		 
				FET_A= 1;
				FET_B =0;
		 }
		 else
		 {
				FET_A= 0;
				FET_B =1;
		 }
	 
	 }
 
	pwm_stepper(&pwm_boost);
	Boost_Ctrl = pwm_is_high(&pwm_boost);

}
soft_timer timer_fetctrl_gen = {0,1,fetctrl_gen};


void timer_Handler()
{	
	soft_timer_stepper(&timer_fetctrl_gen);
}

void exti0_Handler()
{
	
	//printf("exti0_Handler\n");
}

 
void main()
{	
 	Uart_init();
	LCD_Init();
 
	Timer0_set_callback(timer_Handler);
	Timer0_Init();
 
	LCD_Puts(0,0,"DC-AC inverter");
	LCD_Puts(0,1,"220V/50HZ");
	
	while(1)
	{		
		 soft_timer_check(&timer_fetctrl_gen,0);
	}

}
