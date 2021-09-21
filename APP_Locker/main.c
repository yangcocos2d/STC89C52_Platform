#include "LCD1602.h"
#include "UART.h"
#include "timer.h"
#include "software_timer.h"
#include "mystring.h"
#include "BUTTON_Matrix.h"
#include "pwm.h"
#include "AT24CXX.h"

//����������
sbit beep = P2^2 ;

//ʱ�����ơ��û�����ʱ������+����ʱ��+�������̵�ʱ�䡣��λ����
#define MONITOR_INPUT_TIMR_LIMIT 30
#define ALARM_TIME 	60		// 
#define LOCK_TIME		60*5	// 

typedef struct st_time
{
	int sys_second_counter;
	int alarm_dec_counter;
	int button_lock_dec_counter;
}st_time;

st_time st_time_data = {0,0,0};

//ϵͳ��ʼ����...
#define PASSWORD_INIT		"1234"

#define PASSWORD_LEN	4
typedef struct st_password
{
	char password[4];
	int locker_status;
	int error_times;
	int wait_time;
	int is_modify;
}st_password;
st_password st_password_system;
//����û��������ǲ����Ѿ�ƥ����ϵͳ����
int is_match_password(const char* input_str,char *password,int compare_len)
{	int i= 0;
	for(i=0;i<compare_len;i++)
	{
		if(input_str[i] != password[i])
		{
			return 0;
		}
	}

	return 1;
}

typedef struct st_input
{
	unsigned char button_code;
	int button_list_index;
	char button_list[20];

}st_input;
 
st_input idata st_input_btn;
//����������ֶ����
void button_input_clear(st_input* st_input_btn)
{
	int i = 0;
	for(i=0;i<st_input_btn->button_list_index;i++)
	{
		st_input_btn->button_list[i] =  '*';
	}
	st_input_btn->button_list_index = 0;
}

char idata lcd_buffer[2][20];
//LCD��ʾ��ҳ�����
enum lcd_pages {lcd_page_start,lcd_page_show_help,lcd_page_show_input,
								lcd_page_show_welcome,lcd_page_modify_password,
								lcd_page_alarm,lcd_page_lock,lcd_page_end};

enum lcd_pages sys_lcd_pages = lcd_page_show_help;

//Һ����ˢ��...
void lcd_update(void *arg)
{
	//����������Ļ...
	str_copy(lcd_buffer[0],"                ");
	str_copy(lcd_buffer[1],"                ");
	
	//������ʼ����..
	if(sys_lcd_pages == lcd_page_show_help)
	{
		str_copy(lcd_buffer[0],"please input:");
	}
	//��ʾ�û������������...
	else if(sys_lcd_pages == lcd_page_show_input)
	{
		str_copy(lcd_buffer[0],"input:");
		str_copy(&lcd_buffer[0][8],st_input_btn.button_list);
		str_itoa(lcd_buffer[1], st_time_data.sys_second_counter);
		lcd_buffer[1][5] = 's';
	}
	//��ʾ�����Ļ�ӭ����...
	else if(sys_lcd_pages == lcd_page_show_welcome)
	{
		str_copy(lcd_buffer[0],"welcome!");
	}
	//��������...
	else if(sys_lcd_pages == lcd_page_alarm)
	{
		str_copy(lcd_buffer[0],"waring!alarm");
	}
	//���̱���סʱ�Ľ���...
	else if(sys_lcd_pages == lcd_page_lock)
	{
		str_copy(lcd_buffer[0],"button locked!");
		str_itoa(lcd_buffer[1], st_time_data.button_lock_dec_counter);
		lcd_buffer[1][5] = 's';
	}
	//��������ʾ��LCD����
	LCD_Puts(0,0,lcd_buffer[0]);
	LCD_Puts(0,1,lcd_buffer[1]);

	
}

soft_timer idata timer_lcd_update = {0,20,lcd_update};

//������̼��
void button_deal(void *arg)
{
	//�����������ʱ�䵽��...
	if(st_time_data.button_lock_dec_counter == 0)
	{
		st_input_btn.button_code = button_matrix_scan_wait_free();
 
	}
	else
	{
		st_input_btn.button_code = 0;
		
	}
	Uart_puts("button_deal\n");
	Uart_puts(st_input_btn.button_list);
	//����û������˰���...
	if(st_input_btn.button_code!=0)
	{ 
		st_input_btn.button_list[st_input_btn.button_list_index++] = st_input_btn.button_code;
		st_input_btn.button_code = 0;
		sys_lcd_pages = lcd_page_show_input;
		
		
	}
	//������벻ƥ�䣬����������Ч����...
	if(is_match_password(st_input_btn.button_list,st_password_system.password,st_input_btn.button_list_index) == 0)
	{
		button_input_clear(&st_input_btn);
	}
	else
	{
		//����������ֵĳ��ȴ������볤��...
		if(st_input_btn.button_list_index >= PASSWORD_LEN)
		{
			 st_password_system.locker_status = 1;
			 sys_lcd_pages  = lcd_page_show_welcome;
		}
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
