
#include <reg52.h>
#include "pcf8591.h"
#include "timer.h"
#include "software_timer.h"
#include "lcd1602.h"
#include "mystring.h"
#include "button.h"

#define KEY_IS_PRESSED	1
#define FREQ_MIN_SET		1
#define FREQ_MAX_SET		100 
#define VOUT_MAX_SET		50

#define WAVE_ROM_SIZE		256

//ROM点数:256 虚拟表长:512 中断频率:512Hz 中断时间:1.95ms 最高正弦输出频率:512/5 = 102.4
code unsigned char Wave_ROM_Sine[WAVE_ROM_SIZE] = 
{
		128,131,134,137,140,143,146,149,152,155,
		158,162,165,167,170,173,176,179,182,185,
		188,190,193,196,198,201,203,206,208,211,
		213,215,218,220,222,224,226,228,230,232,
		234,235,237,238,240,241,243,244,245,246,
		248,249,250,250,251,252,253,253,254,254,
		254,255,255,255,255,255,255,255,254,254,
		254,253,253,252,251,250,250,249,248,246,
		245,244,243,241,240,238,237,235,234,232,
		230,228,226,224,222,220,218,215,213,211,
		208,206,203,201,198,196,193,190,188,185,
		182,179,176,173,170,167,165,162,158,155,
		152,149,146,143,140,137,134,131,128,124,
		121,118,115,112,109,106,103,100,97,93,
		90,88,85,82,79,76,73,70,67,65,
		62,59,57,54,52,49,47,44,42,40,
		37,35,33,31,29,27,25,23,21,20,
		18,17,15,14,12,11,10,9, 7, 6,
		5, 5, 4, 3, 2, 2, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 1, 2,
		2, 3, 4, 5, 5, 6, 7, 9, 10,11,
		12,14,15,17,18,20,21,23,25,27,
		29,31,33,35,37,40,42,44,47,49,
		52,54,57,59,62,65,67,70,73,76,
		79,82,85,88,90,93,97,100,103,106,
		109,112,115,118,121,124

};

typedef struct dds_block
{
	int freq;
	unsigned long data_index;
	int rom_len;
	
	unsigned char Rom_data;
	unsigned int  Vout_zoom;
	enum wave_type {sin = 1,sawtooth,triangle,rectangle} wave;
	int Check;
	
}dds_block;

dds_block idata wave_dds = {100,0,512,0,50,sin,128};

char idata LCD_disbuffer[2][16];

void LCD_UpdatePara()
{
	int freq_dis = wave_dds.freq;
	int Vout_zoom_dis = wave_dds.Vout_zoom;
	
	if(wave_dds.wave == sin)
	{
		str_copy(&LCD_disbuffer[0][5],"sin");
	}
	if(wave_dds.wave == sawtooth)
	{
		str_copy(&LCD_disbuffer[0][5],"saw");
	}
	if(wave_dds.wave == triangle)
	{
		str_copy(&LCD_disbuffer[0][5],"tri");
	}
	if(wave_dds.wave == rectangle)
	{
		str_copy(&LCD_disbuffer[0][5],"rec");
	}
	
	LCD_disbuffer[1][5] = freq_dis/100 + '0';
	freq_dis = freq_dis%100;
	LCD_disbuffer[1][6] = freq_dis/10 + '0';
	freq_dis = freq_dis%10;
	LCD_disbuffer[1][7] = freq_dis + '0';
	
	LCD_disbuffer[0][12] = Vout_zoom_dis/10 + '0';
	Vout_zoom_dis = Vout_zoom_dis%10;
	LCD_disbuffer[0][13] = '.';
	LCD_disbuffer[0][14] = Vout_zoom_dis + '0';

}

void LCD_Update()
{
	Timer0_interupt_disable();
	
	str_copy(LCD_disbuffer[0],"WAVE:    V:    v");
	str_copy(LCD_disbuffer[1],"FREQ:   HZ      ");

	LCD_UpdatePara();
	
	LCD_Puts(0,0,LCD_disbuffer[0]);
	LCD_Puts(0,1,LCD_disbuffer[1]);
	
	Timer0_interupt_enable();
}

//设置最高电压 单位0.1V
void SetWaveVoutZoom(unsigned int Vout)
{
	unsigned int data_set = (Vout<<8)/5;
	
	data_set = (data_set + 5)/10;
	if(data_set > (WAVE_ROM_SIZE - 1))
	{
		data_set = (WAVE_ROM_SIZE-1);
	}
	Timer0_interupt_disable();
	pcf8591_set_dac_value(PCF8591_SLAVE_ADDRESS_1,(unsigned char)data_set);
	Timer0_interupt_enable();

}

void timer_key_update_handle()
{
	char is_key_press = !KEY_IS_PRESSED ;

	if(BUTTON_LONG_PRESS == button_get_status(BUTTON_ESC,5))
	{	
		is_key_press = KEY_IS_PRESSED;
		wave_dds.wave++;
		if(wave_dds.wave > rectangle)
		{
			wave_dds.wave = sin;
		}
	}
	if(BUTTON_LONG_PRESS == button_get_status(BUTTON_UP,2))
	{	
		is_key_press = KEY_IS_PRESSED;
		wave_dds.freq++;
		if(wave_dds.freq > FREQ_MAX_SET)
		{
			wave_dds.freq = FREQ_MIN_SET;
		}
	}
	if(BUTTON_LONG_PRESS == button_get_status(BUTTON_DOWN,2))
	{	
		is_key_press = KEY_IS_PRESSED;
		wave_dds.freq--;
		if(wave_dds.freq < FREQ_MIN_SET)
		{
			wave_dds.freq = FREQ_MAX_SET;
		}
	}
	if(BUTTON_LONG_PRESS == button_get_status(BUTTON_ENTER,2))
	{	
		is_key_press = KEY_IS_PRESSED;
		wave_dds.Vout_zoom++;
		if(wave_dds.Vout_zoom > VOUT_MAX_SET)
		{
			wave_dds.Vout_zoom = 0;
		}
		SetWaveVoutZoom(wave_dds.Vout_zoom);
	}
	if(is_key_press == KEY_IS_PRESSED)
	{	
		LCD_Update();
		is_key_press = !KEY_IS_PRESSED;
	}
	
}

soft_timer idata timer_key_update = {0,10,timer_key_update_handle};

void Timer0_Handle()
{		 
	//static int rom_cnt = 0 ;
	wave_dds.data_index += wave_dds.freq;
	if(wave_dds.data_index >= wave_dds.rom_len)
	{
		wave_dds.data_index-= wave_dds.rom_len;
	}
	
	if(wave_dds.wave == sin)
	{
		wave_dds.Rom_data = Wave_ROM_Sine[wave_dds.data_index>>1];
	}
	if(wave_dds.wave == sawtooth)
	{
		wave_dds.Rom_data = wave_dds.data_index >> 1;
	}			
	if(wave_dds.wave == triangle)
	{
		if(wave_dds.data_index < (wave_dds.rom_len>>1))
		{
			wave_dds.Rom_data = wave_dds.data_index;
		}
		else
		{
			wave_dds.Rom_data =  (WAVE_ROM_SIZE-1) - (wave_dds.data_index - WAVE_ROM_SIZE);
		}
	}
	if(wave_dds.wave == rectangle)
	{
		if(wave_dds.data_index < (wave_dds.rom_len>>1))
		{
			wave_dds.Rom_data = 0;
		}
		else
		{
			wave_dds.Rom_data = (WAVE_ROM_SIZE-1);
		}
	}	
	/*pcf8591_set_dac_value(PCF8591_SLAVE_ADDRESS_0,Wave_ROM_Sine[rom_cnt++]);
	if(rom_cnt >= WAVE_ROM_SIZE)
	{
		rom_cnt = 0;
	}*/
	pcf8591_set_dac_value(PCF8591_SLAVE_ADDRESS_0,wave_dds.Rom_data);
	soft_timer_stepper(&timer_key_update);
}
sbit test = P1^7;
void main()
{
	test = 1;
	LCD_Init();
	Timer0_set_callback(Timer0_Handle);
	Timer0_Init();
	LCD_Update();
	SetWaveVoutZoom(wave_dds.Vout_zoom);
	test = 0;
	while(1)
	{
		if(soft_timer_check(&timer_key_update) == 1)
		{ 
			timer_key_update.fun();
		}
	}

}

