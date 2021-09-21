#include "LCD1602.h"
#include "UART.h"
#include "timer.h"
#include "software_timer.h"
#include "mystring.h"
#include "BUTTON_Matrix.h"
#include "pwm.h"
#include "AT24CXX.h"

  
typedef struct st_input
{
	unsigned char button_code;
	int button_list_index;
	char button_list[20];

}st_input;
 
st_input idata st_input_btn;
 

char idata lcd_buffer[2][20];
 

 

//Һ����ˢ��...
void lcd_update(void *arg)
{
	//����������Ļ...
	str_copy(lcd_buffer[0],"                ");
	str_copy(lcd_buffer[1],"                ");
	
	 
	//��������ʾ��LCD����
	LCD_Puts(0,0,lcd_buffer[0]);
	LCD_Puts(0,1,lcd_buffer[1]);

	
}

soft_timer idata timer_lcd_update = {0,20,lcd_update};

//������̼��
void button_deal(void *arg)
{
	//�����������ʱ�䵽��...
	 
		st_input_btn.button_code = button_matrix_scan_wait_free();
 
	 
	if(st_input_btn.button_code!=0)
	{ 
		st_input_btn.button_list[st_input_btn.button_list_index++] = st_input_btn.button_code;
		st_input_btn.button_code = 0;
 
		
		
	}
	 

	 
	
}

soft_timer idata timer_button_deal = {0,2,button_deal};


pwm idata pwm_beep = {50,2,0};
void generate_pwm()
{
	//����������
	pwm_stepper(&pwm_beep);
	//���������������ұ�������ʱ��û��0���ͷ���������...
	if(st_password_system.error_times > 0 && st_time_data.alarm_dec_counter > 0)
	{
		beep = 0;//pwm_is_high(&pwm_beep);
	}
	else
	{
		beep = 1;
	}
	
}
soft_timer idata timer_generate_pwm = {0,1,generate_pwm};

void second_counter()
{
	//����ʱ
	if(sys_lcd_pages == lcd_page_show_input)
	{
		st_time_data.sys_second_counter++;
	}
	 

	if(st_time_data.alarm_dec_counter > 0)
	{
		st_time_data.alarm_dec_counter--;
	}
	
	if(st_time_data.button_lock_dec_counter > 0)
	{
		st_time_data.button_lock_dec_counter--;
	}
}
soft_timer idata timer_second_counter = {0,100,second_counter};


void user_monitor()
{
	//����ʾ"welcome"�Ľ���ʱ...
	if(sys_lcd_pages  == lcd_page_show_welcome)
	{
			st_time_data.sys_second_counter = 0;
			st_password_system.error_times = 0;
			st_time_data.alarm_dec_counter = 0;
			st_time_data.button_lock_dec_counter = 0;
	}
	else
	{
		//����û�����ʱ�����������ʱ��...
		if( st_time_data.sys_second_counter > MONITOR_INPUT_TIMR_LIMIT)
		{
			st_password_system.error_times++;
			st_time_data.sys_second_counter = 0;
			st_time_data.alarm_dec_counter = ALARM_TIME;
		}
		//����û���������������3��...
		if(st_password_system.error_times >= 3)
		{
			st_time_data.button_lock_dec_counter = LOCK_TIME;
			st_password_system.error_times = 0;
		}
		//�����������ʱ��û����...
		if(st_time_data.alarm_dec_counter > 0)
		{
			sys_lcd_pages = lcd_page_alarm;
		}
		else
		{
			sys_lcd_pages = lcd_page_show_input;
		}
		//���������������ʱ��û����..
		if(st_time_data.button_lock_dec_counter > 0)
		{
			sys_lcd_pages = lcd_page_lock;
		}
		
	}
	
}
soft_timer idata timer_user_monitor = {0,10,user_monitor};

//��ʱ�����������ƽ�һ�����ġ�
void timer_Handler()
{	
	soft_timer_stepper(&timer_lcd_update);
	soft_timer_stepper(&timer_button_deal);
	soft_timer_stepper(&timer_generate_pwm);	
	soft_timer_stepper(&timer_second_counter);
	soft_timer_stepper(&timer_user_monitor);
}

void exti0_Handler()
{
	
 
}
//�����ȡ����AT24C02��ȡ����
void password_read()
{
	AT24CXX_read(0,st_password_system.password,PASSWORD_LEN);
	if(st_password_system.password[0]>='9' || st_password_system.password[0]<='0' )
	{
		str_copy(st_password_system.password,PASSWORD_INIT);
	}
}
 
//���뱣�棬���������AT24C02
void password_save()
{
	AT24CXX_write(0,st_password_system.password,PASSWORD_LEN);
}

void main()
{	
 
	
 	Uart_init();//����ͨ�ų�ʼ��
	LCD_Init();	//LCDҺ������ʼ��
	Timer0_set_callback(timer_Handler);//��ʱ��0��������ʼ��
	Timer0_Init();//��ʱ��0��ʼ��
	password_read();//�Ӵ洢��AT24C02�ж�ȡ����
	
	while(1)
	{		
		soft_timer_check(&timer_lcd_update,0);	//���ǲ��Ǹ�LCDˢ����Ļ�ˣ�����ǣ�ִ��LCDˢ��
		soft_timer_check(&timer_button_deal,0); //���ǲ��Ǹü�ⰴ����
		soft_timer_check(&timer_generate_pwm,0);//���ǲ��Ǹø�������������
		soft_timer_check(&timer_second_counter,0);//ʱ���ۼӺ���������ʱ��
		soft_timer_check(&timer_user_monitor,0);//���ǲ��Ǹü���û�������ʱ����
	}

}
