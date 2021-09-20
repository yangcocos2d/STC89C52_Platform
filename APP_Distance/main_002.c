
#include <stdio.h>
#include <reg52.h>
#include "timer.h"
#include "software_timer.h"
#include "lcd1602.h"
#include "mystring.h"
#include "UART.h"
#include "exti_interrupt.h"
#include "motor_stepper.h"
#include <intrins.h>
 
sbit test = P1^7;
sbit sonic_echo = P3^3;
sbit sonic_1 = P2^4;
sbit alarm = P1^5;
#define ALALM_DISTANCE	100
#define STOP_DISTANCE		30


typedef struct sonic_time_record
{
	unsigned int timer0_res_load_start;
	unsigned int timer0_res_load_end;
	unsigned int timer0_res_load_diff;
	int distance;
}sonic_time_record;

sonic_time_record sonic_time_record_data = {0,0,0,ALALM_DISTANCE*2};
 
void startup_delay10us( )
{
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();
}

 
enum lcd_pages{lcd_page_start,lcd_page_sonic,lcd_page_alarm,lcd_page_end};
enum lcd_pages lcd_page = lcd_page_sonic;
char idata lcd_buffer[2][20];
void lcd_update()
{
	
	float f_distance = sonic_time_record_data.timer0_res_load_diff*0.037425/2;
	sonic_time_record_data.distance = f_distance + 0.5;
	
	str_copy(lcd_buffer[0],"                ");
	str_copy(lcd_buffer[1],"                ");

	if(lcd_page == lcd_page_sonic)
	{
		str_copy(lcd_buffer[0],"sonic (cm):");
		str_itoa(lcd_buffer[1], sonic_time_record_data.distance);
	}
	else if(lcd_page == lcd_page_alarm)
	{
		str_copy(lcd_buffer[0],"alarm!");
		str_itoa(lcd_buffer[1], sonic_time_record_data.distance);
	}
	LCD_Puts(0,0,lcd_buffer[0]);
	LCD_Puts(0,1,lcd_buffer[1]);
	 

}
soft_timer idata timer_lcd_update = {0,20,lcd_update};

void sonic_measure()
{
	if(sonic_echo == 0)
	{
		sonic_1 = 1;
		startup_delay10us( );
		sonic_1 = 0;
	}
	
}
soft_timer idata timer_sonic_measure = {0,10,sonic_measure};


void alarm_manage()
{
	if(sonic_time_record_data.distance <= ALALM_DISTANCE)
	{
		lcd_page = lcd_page_alarm;
		alarm = ~alarm;
	 
	}
	else
	{
	  lcd_page = lcd_page_sonic;
		alarm = 1;
	}
 
}
soft_timer idata timer_alarm_manage = {0,2,alarm_manage};

void motor_stepper()
{

	if(sonic_time_record_data.distance <= STOP_DISTANCE)
	{
		
		motor_stepper_fast_stop();
	}
	else
	{
		
		motor_stepper_clockwise_once();
	}
	
 
	
}
soft_timer idata timer_motor_stepper = {0,10,motor_stepper};

void Timer0_Handle()
{

	soft_timer_stepper(&timer_lcd_update);
	soft_timer_stepper(&timer_sonic_measure);
	soft_timer_check(&timer_sonic_measure,0);
	
	soft_timer_stepper(&timer_motor_stepper);
	soft_timer_stepper(&timer_alarm_manage);
	
}

void exti0_Handle()
{
	//Timer0_interupt_disable();
	sonic_time_record_data.timer0_res_load_start = TH0*256 + TL0;
}
void exti1_Handle()
{
	sonic_time_record_data.timer0_res_load_end = TH0*256 + TL0;
	//Timer0_interupt_enable();
	if(sonic_time_record_data.timer0_res_load_end >= sonic_time_record_data.timer0_res_load_start)
	{
			sonic_time_record_data.timer0_res_load_diff = sonic_time_record_data.timer0_res_load_end - sonic_time_record_data.timer0_res_load_start;
	}
	else
	{	
			sonic_time_record_data.timer0_res_load_diff = sonic_time_record_data.timer0_res_load_end + 63336 - sonic_time_record_data.timer0_res_load_start;
	}
	 
}
void main()
{
 
	LCD_Init();
	Uart_init();
	Timer0_set_callback(Timer0_Handle);
	Timer0_Init();
	exti0_interrupt_set_callback(exti0_Handle);
	exti1_interrupt_set_callback(exti1_Handle);
	exti_interrupt_init();
  
	while(1)
	{
		soft_timer_check(&timer_lcd_update,0);
		
		soft_timer_check(&timer_motor_stepper,0);
		soft_timer_check(&timer_alarm_manage,0);
	}

	 
	 
}

