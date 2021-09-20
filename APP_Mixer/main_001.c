#include "LCD1602.h"
#include "UART.h"
#include "button.h"
#include "motor_encode.h"
#include "timer.h"
#include "pwm.h"
#include "software_timer.h"
#include "mystring.h"
#include "exti_interrupt.h"
#include "iic.h"
#include "pcf8591.h"
#include "ds18b20.h"
 
#define MOTOR_STOP  0
#define MOTOR_RUN   1
#define MOTOR_FORCE_STOP 2

sbit alarm = P1^4;
sbit test = P1^7;
pwm   motor_pwm = {50,10,0};
static int idata second_counter_data = 0;
unsigned int idata press_01=0,press_02 = 0;
int motor_force_stop_flag = MOTOR_RUN; 
int motor_run_flag = MOTOR_RUN;
unsigned int ds18b20_tempature = 0;

 
enum lcd_pages{start_enum=0,show_motor_speed,show_time,show_press,show_tem,end_enum};
enum lcd_pages lcd_pages_info_display = show_motor_speed;

char idata lcd_display[2][20];
void lcd_info_update(void *arg)
{
	int temp = 0;
	Uart_puts("lcd_info_update\n");
	str_copy(lcd_display[0],"                ");
	str_copy(lcd_display[1],"                ");
	if(lcd_pages_info_display == show_motor_speed)
	{
		str_copy(lcd_display[0],"motor run!");
		str_copy(lcd_display[1],"speed=   %");
		str_itoa(&lcd_display[1][6],motor_pwm.pwm_high_percentage);
		
	}
	else if(lcd_pages_info_display == show_time)
	{
		
	  str_copy(lcd_display[0],"run time(s):");
		str_itoa(&lcd_display[1][0],second_counter_data);
		lcd_display[1][15] = 's';
		
	}
	else if(lcd_pages_info_display == show_press)
	{

		str_copy(lcd_display[0],"press 1: ");
		str_copy(lcd_display[1],"press 2: ");
		str_itoa(&lcd_display[0][10],press_01);
		str_itoa(&lcd_display[1][10],press_02);

	}
	else if(lcd_pages_info_display == show_tem)
	{
		unsigned int tem = 0;
	  tem = ds18b20_tempature; 
		tem = tem/10;
		str_copy(lcd_display[0],"temperature:  ");
	  str_itoa(&lcd_display[1][0],tem);
	  lcd_display[1][4] = 0xDF;
		lcd_display[1][5] = 'C';
 
	}
	
	
	LCD_Puts(0,0,lcd_display[0]);
	LCD_Puts(0,1,lcd_display[1]);
	 

}
soft_timer timer_lcd = {0,50,lcd_info_update};


#define BUTTON_PRESS_TIME  10
void button_event_deal(void *arg)
{
	Uart_puts("button_event_deal\n");
 
	//在设置速度的页面...
  if(lcd_pages_info_display == show_motor_speed)
	{
		if(BUTTON_LONG_PRESS ==button_get_status(BUTTON_UP,BUTTON_PRESS_TIME))
		{
			if(motor_pwm.pwm_high_percentage<100)
			{
				motor_pwm.pwm_high_percentage++;
			}
			
		}
		
		if(BUTTON_LONG_PRESS ==button_get_status(BUTTON_DOWN,BUTTON_PRESS_TIME))
		{
			if(motor_pwm.pwm_high_percentage>0)
			{
				motor_pwm.pwm_high_percentage--;
			}
			
		}
	}
	
	//在设置搅拌时间的页面...
	if(lcd_pages_info_display == show_time)
	{
		if(BUTTON_LONG_PRESS ==button_get_status(BUTTON_UP,BUTTON_PRESS_TIME))
		{
			 second_counter_data+=10;
			
		}
		
		if(BUTTON_LONG_PRESS ==button_get_status(BUTTON_DOWN,BUTTON_PRESS_TIME))
		{
			 
			if(second_counter_data>0)
			{
				second_counter_data--;
			}
		}
	}
	
	//所有页面...
	if(BUTTON_LONG_PRESS ==button_get_status(BUTTON_ENTER,BUTTON_PRESS_TIME))
	{		 
			lcd_pages_info_display++;
			if(lcd_pages_info_display >= end_enum)
			{
				lcd_pages_info_display = start_enum + 1;
			}
 
			motor_force_stop_flag = MOTOR_RUN; 
		 
	}
	
	if(BUTTON_LONG_PRESS  ==button_get_status(BUTTON_ESC,BUTTON_PRESS_TIME))
	{
			motor_encode_faster_stop();
			motor_force_stop_flag = MOTOR_FORCE_STOP;
	 
	}
}
soft_timer timer_button = {0,1,button_event_deal};
 
void second_counter(void *arg)
{
	Uart_puts("second_counter\n");
	second_counter_data++;
 
	
}
soft_timer timer_minute_counter = {0,100,second_counter};

void get_ds18b20_tempature(void *arg)
{
	Uart_puts("get_ds18b20_tempature\n");
	ds18b20_tempature = ds18b20_get_temp();
	ds18b20_start_get_temp();
	
}
soft_timer timer_get_ds18b20_tempature = {0,300,get_ds18b20_tempature};
/*
电机管理策略:
press_01:搅拌罐压力
press_02:传送带压力

搅拌罐压力代表投入物料，传送带压力代表物料破碎成功。

开启电机的时机:
搅拌罐压力增大，代表有物料投放。
关闭电机的时机：
搅拌罐没有压力变化；
或者传送带长时间没有产生压力变化,代表一直没有磨碎的物料流出，这代表搅拌罐堵死了。

*/
void motor_manage(void *arg)
{
  Uart_puts("motor_manage\n");
	press_01 =  pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_0,0);
	press_01 =  pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_0,1);
	press_02 =  pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_0,0);
	
  
	
	if( press_01 >= 100)
	{
		motor_run_flag = MOTOR_RUN;
		
	}
	else
	{
		motor_run_flag = MOTOR_STOP;
	}
 
}
soft_timer timer_motor_manage = {0,20,motor_manage};

void timer_Handler()
{	
	int pwm_status = 0;
	 
	
	pwm_stepper(&motor_pwm);
	pwm_status = pwm_is_high(&motor_pwm);

	if(motor_force_stop_flag == MOTOR_FORCE_STOP)
	{
			motor_encode_faster_stop();
		 alarm = 0;
	}
	else
	{
		if(motor_run_flag == MOTOR_RUN)
		{
			if(pwm_status == 0)
			{
				motor_encode_free_stop();
			}
			else
			{
				motor_encode_clockwise();
			}
			alarm = 1;
		}
		else
		{
			 motor_encode_faster_stop();
			 alarm = 0;
		}
	
	}
	
	

	soft_timer_stepper(&timer_lcd);
	soft_timer_stepper(&timer_button);
	soft_timer_stepper(&timer_minute_counter);
	soft_timer_stepper(&timer_motor_manage );
	soft_timer_stepper(&timer_get_ds18b20_tempature );
}

void exti0_Handler()
{
	
	//printf("exti0_Handler\n");
}

void main()
{	
 	Uart_init();
	LCD_Init();
	exti_interrupt_set_callback(exti0_Handler);
	exti_interrupt_init();
	Timer0_set_callback(timer_Handler);
	Timer0_Init();
	motor_encode_enable();
	motor_encode_faster_stop();
	
	 
	while(1)
	{			
		soft_timer_check(&timer_lcd,0);
		soft_timer_check(&timer_button,0);
		soft_timer_check(&timer_minute_counter,0);
		soft_timer_check(&timer_motor_manage,0);
		soft_timer_check(&timer_get_ds18b20_tempature,0);
 
		Uart_puts("while 1 loop\n"); 

	}

 
}
