#include "LCD1602.h"
#include "UART.h"
#include "timer.h"
#include "software_timer.h"
#include "mystring.h"
#include "BUTTON_Matrix.h"
#include "pwm.h"
#include "AT24CXX.h"

//蜂鸣器报警
sbit beep = P2^2 ;

//时间限制。用户输入时间限制+报警时间+锁定键盘的时间。单位，秒
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

//系统初始密码...
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
//检查用户的输入是不是已经匹配了系统密码
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
//把输入的数字都清空
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
//LCD显示的页面汇总
enum lcd_pages {lcd_page_start,lcd_page_show_help,lcd_page_show_input,
								lcd_page_show_welcome,lcd_page_modify_password,
								lcd_page_alarm,lcd_page_lock,lcd_page_end};

enum lcd_pages sys_lcd_pages = lcd_page_show_help;

//液晶屏刷屏...
void lcd_update(void *arg)
{
	//首先清理屏幕...
	str_copy(lcd_buffer[0],"                ");
	str_copy(lcd_buffer[1],"                ");
	
	//开机初始界面..
	if(sys_lcd_pages == lcd_page_show_help)
	{
		str_copy(lcd_buffer[0],"please input:");
	}
	//提示用户输入密码界面...
	else if(sys_lcd_pages == lcd_page_show_input)
	{
		str_copy(lcd_buffer[0],"input:");
		str_copy(&lcd_buffer[0][8],st_input_btn.button_list);
		str_itoa(lcd_buffer[1], st_time_data.sys_second_counter);
		lcd_buffer[1][5] = 's';
	}
	//显示开锁的欢迎界面...
	else if(sys_lcd_pages == lcd_page_show_welcome)
	{
		str_copy(lcd_buffer[0],"welcome!");
	}
	//报警界面...
	else if(sys_lcd_pages == lcd_page_alarm)
	{
		str_copy(lcd_buffer[0],"waring!alarm");
	}
	//键盘被锁住时的界面...
	else if(sys_lcd_pages == lcd_page_lock)
	{
		str_copy(lcd_buffer[0],"button locked!");
		str_itoa(lcd_buffer[1], st_time_data.button_lock_dec_counter);
		lcd_buffer[1][5] = 's';
	}
	//把文字显示在LCD上面
	LCD_Puts(0,0,lcd_buffer[0]);
	LCD_Puts(0,1,lcd_buffer[1]);

	
}

soft_timer idata timer_lcd_update = {0,20,lcd_update};

//矩阵键盘检查
void button_deal(void *arg)
{
	//如果键盘锁定时间到了...
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
	//如果用户输入了按键...
	if(st_input_btn.button_code!=0)
	{ 
		st_input_btn.button_list[st_input_btn.button_list_index++] = st_input_btn.button_code;
		st_input_btn.button_code = 0;
		sys_lcd_pages = lcd_page_show_input;
		
		
	}
	//如果密码不匹配，即输入了无效数字...
	if(is_match_password(st_input_btn.button_list,st_password_system.password,st_input_btn.button_list_index) == 0)
	{
		button_input_clear(&st_input_btn);
	}
	else
	{
		//如果输入数字的长度大于密码长度...
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
	//蜂鸣器报警
	pwm_stepper(&pwm_beep);
	//如果密码输入错误，且报警倒计时还没归0，就蜂鸣器报警...
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
	//秒表计时
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
	//在显示"welcome"的界面时...
	if(sys_lcd_pages  == lcd_page_show_welcome)
	{
			st_time_data.sys_second_counter = 0;
			st_password_system.error_times = 0;
			st_time_data.alarm_dec_counter = 0;
			st_time_data.button_lock_dec_counter = 0;
	}
	else
	{
		//如果用户输入时间大于了限制时间...
		if( st_time_data.sys_second_counter > MONITOR_INPUT_TIMR_LIMIT)
		{
			st_password_system.error_times++;
			st_time_data.sys_second_counter = 0;
			st_time_data.alarm_dec_counter = ALARM_TIME;
		}
		//如果用户输入密码错误大于3次...
		if(st_password_system.error_times >= 3)
		{
			st_time_data.button_lock_dec_counter = LOCK_TIME;
			st_password_system.error_times = 0;
		}
		//如果报警倒计时还没结束...
		if(st_time_data.alarm_dec_counter > 0)
		{
			sys_lcd_pages = lcd_page_alarm;
		}
		else
		{
			sys_lcd_pages = lcd_page_show_input;
		}
		//如果键盘锁定倒计时还没结束..
		if(st_time_data.button_lock_dec_counter > 0)
		{
			sys_lcd_pages = lcd_page_lock;
		}
		
	}
	
}
soft_timer idata timer_user_monitor = {0,10,user_monitor};

//定时器服务函数。推进一个节拍。
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
//密码读取，从AT24C02读取密码
void password_read()
{
	AT24CXX_read(0,st_password_system.password,PASSWORD_LEN);
	if(st_password_system.password[0]>='9' || st_password_system.password[0]<='0' )
	{
		str_copy(st_password_system.password,PASSWORD_INIT);
	}
}
 
//密码保存，把密码存入AT24C02
void password_save()
{
	AT24CXX_write(0,st_password_system.password,PASSWORD_LEN);
}

void main()
{	
 
	
 	Uart_init();//串口通信初始化
	LCD_Init();	//LCD液晶屏初始化
	Timer0_set_callback(timer_Handler);//定时器0服务函数初始化
	Timer0_Init();//定时器0初始化
	password_read();//从存储器AT24C02中读取密码
	
	while(1)
	{		
		soft_timer_check(&timer_lcd_update,0);	//看是不是该LCD刷新屏幕了，如果是，执行LCD刷屏
		soft_timer_check(&timer_button_deal,0); //看是不是该检测按键了
		soft_timer_check(&timer_generate_pwm,0);//看是不是该给蜂鸣器报警了
		soft_timer_check(&timer_second_counter,0);//时间累加函数。秒表计时器
		soft_timer_check(&timer_user_monitor,0);//看是不是该监测用户的输入时间了
	}

}
