
#include <reg52.h>
#include "pcf8591.h"
#include "74hc573.h"
#include "timer.h"
#include "software_timer.h"
#include "lcd1602.h"
#include "mystring.h"
#include "button.h"
#include "UART.h"

sbit test = P1^7;
sbit alarm = P1^4;

/*
电源的开关控制


*/
enum power_status{power_normal,power_over,power_low};

typedef struct Power_data
{
	unsigned char power_voltage_low_limit;
	unsigned char power_voltage_high_limit;
	
	unsigned char power_main_a_voltage;
	unsigned char power_main_b_voltage;
	unsigned char power_main_c_voltage;
	
	unsigned char power_standby_a_voltage;
	unsigned char power_standby_b_voltage;
	unsigned char power_standby_c_voltage;

	enum power_status power_main_status;
	enum power_status power_standby_status;
	
}Power_data;

#define POWER_MAIN  		0
#define POWER_STANDBY  	1
//3V=>153
Power_data cur_power_data = {110,204,0,0,0,0,0,0,power_normal,power_normal};


void power_open(int power_no)
{
	locker_resetbit(LOCKER_1,power_no);
}
void power_close(int power_no)
{
	locker_setbit(LOCKER_1,power_no);
}
void power_led(int power_no,enum power_status status)
{
		locker_setbit(LOCKER_0,0 + power_no*3);
		locker_setbit(LOCKER_0,1 + power_no*3);
		locker_setbit(LOCKER_0,2 + power_no*3);
	
		if( status == power_normal)
		{
			locker_resetbit(LOCKER_0,0 + power_no*3);
		}
		if( status == power_over)
		{
			locker_resetbit(LOCKER_0,1 + power_no*3);
		}
		if( status == power_low)
		{
			locker_resetbit(LOCKER_0,2 + power_no*3);
		}
}

void power_data_get(Power_data* cur_power_data)
{
	cur_power_data->power_main_a_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_0,0);
	cur_power_data->power_main_a_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_0,1);
	cur_power_data->power_main_b_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_0,2);
	cur_power_data->power_main_c_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_0,0);

	cur_power_data->power_standby_a_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_1,0);
	cur_power_data->power_standby_a_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_1,1);
	cur_power_data->power_standby_b_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_1,2);
	cur_power_data->power_standby_c_voltage = pcf8591_get_adc_value(PCF8591_SLAVE_ADDRESS_1,0);

}

/*
LCD页面


*/ 
enum lcd_page
{
 
lcd_page_start = 0,
lcd_page_show_power_main_A,
lcd_page_show_power_main_B,
lcd_page_show_power_main_C,

lcd_page_show_power_standby_A,
lcd_page_show_power_standby_B,
lcd_page_show_power_standby_C,
lcd_page_end
};

enum lcd_page lcd_page_no = lcd_page_show_power_main_A;
char idata lcd_buffer[2][18];
void lcd_update(void *arg)
{
		str_copy(lcd_buffer[0],"                ");
		str_copy(lcd_buffer[1],"                ");
		
	
		 
		if(lcd_page_no == lcd_page_show_power_main_A)
		{
			str_copy(lcd_buffer[0],"power-main A:");
			str_itoa(lcd_buffer[1],cur_power_data.power_main_a_voltage);
 
		}
		else if(lcd_page_no == lcd_page_show_power_main_B)
		{
			str_copy(lcd_buffer[0],"power-main B:");
			str_itoa(lcd_buffer[1],cur_power_data.power_main_b_voltage);
 
		}
		else if(lcd_page_no == lcd_page_show_power_main_C)
		{
			str_copy(lcd_buffer[0],"power-main C:");
			str_itoa(lcd_buffer[1],cur_power_data.power_main_c_voltage);
		
		} 
		else if(lcd_page_no == lcd_page_show_power_standby_A)
		{
			str_copy(lcd_buffer[0],"power-standby A:");
			str_itoa(lcd_buffer[1],cur_power_data.power_standby_a_voltage);
		}
		else if(lcd_page_no == lcd_page_show_power_standby_B)
		{
			str_copy(lcd_buffer[0],"power-standby B:");
			str_itoa(lcd_buffer[1],cur_power_data.power_standby_b_voltage);
		}
		else if(lcd_page_no == lcd_page_show_power_standby_C)
		{
			str_copy(lcd_buffer[0],"power-standby C:");
			str_itoa(lcd_buffer[1],cur_power_data.power_standby_c_voltage);
		}
	 
		LCD_Puts(0,0,lcd_buffer[0]);
		LCD_Puts(0,1,lcd_buffer[1]);
 
}
soft_timer timer_lcd_update = {0,20,lcd_update};
 
/*
按钮响应


*/
void button_deal(void *arg)
{

		if(button_get_status(BUTTON_ENTER,20) == BUTTON_LONG_PRESS)
		{
	 
			lcd_page_no++;
			if(lcd_page_no >= lcd_page_end)
			{
				lcd_page_no = lcd_page_start + 1;
			}
 
		}
		
}
soft_timer timer_button_deal = {0,1,button_deal};

/*
电源管理策略


*/
void power_manage(void *arg)
{
	 
	power_data_get(&cur_power_data);
	if(	cur_power_data.power_main_b_voltage > cur_power_data.power_voltage_high_limit)
	{
		cur_power_data.power_main_status = power_over;
 

	}
	else if(cur_power_data.power_main_b_voltage < cur_power_data.power_voltage_low_limit )
	{
		cur_power_data.power_main_status = power_low;
 

	}
	else
	{	
		cur_power_data.power_main_status = power_normal;
		
	}
	power_led(POWER_MAIN,cur_power_data.power_main_status);

	if(	cur_power_data.power_standby_b_voltage > cur_power_data.power_voltage_high_limit)
	{
		cur_power_data.power_standby_status = power_over;
	}
	else if(cur_power_data.power_standby_b_voltage < cur_power_data.power_voltage_low_limit )
	{
		cur_power_data.power_standby_status = power_low;
	}
	else
	{
		cur_power_data.power_standby_status = power_normal;
		
	}
	power_led(POWER_STANDBY,cur_power_data.power_standby_status);

	if(	cur_power_data.power_main_status != power_normal 
		&& cur_power_data.power_standby_status != power_normal)
	{
		power_close(POWER_MAIN);
		power_close(POWER_STANDBY);		
		alarm = 0;
	}
	else if(cur_power_data.power_main_status == power_normal)
	{
		power_open(POWER_MAIN);
		power_close(POWER_STANDBY);
		alarm = 1;
	}
	else if(cur_power_data.power_standby_status == power_normal)
	{
		power_close(POWER_MAIN);
		power_open(POWER_STANDBY);
		alarm = 1;
	}
 
}
soft_timer timer_power_manage = {0,100,power_manage};

void Timer0_Handle()
{

	soft_timer_stepper(&timer_lcd_update);
	soft_timer_stepper(&timer_button_deal);
	soft_timer_stepper(&timer_power_manage);
}
 
void startup_delayus(unsigned int us)
{
	 unsigned int i = us /13;
	 while(i--);
}
void startup_delayms(unsigned int ms)
{
	 unsigned int i = ms;
	 while(i--)
	 {
		 startup_delayus(1000);
	 }
}

void main()
{
	
	locker_datset(LOCKER_0,0XFF);
	locker_datset(LOCKER_1,0XFF);
	LCD_Init();
	Uart_init();
	Timer0_set_callback(Timer0_Handle);
	Timer0_Init();

	 
	while(1)
	{
		 Uart_puts("main loop\n");
		 soft_timer_check(&timer_button_deal,0);
		 soft_timer_check(&timer_lcd_update,0);
		 soft_timer_check(&timer_power_manage,0);
	 
	}

	 
	while(1);
}

