#include "ds18b20.h"
#include<intrins.h>

#define TIME_OUT 100

void ds18b20_delayms(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
		for(j=120;j>0;j--);
}

void ds18b20_delayus(unsigned char  us) 
{
	while(us--);
}

void ds18b20_init()
{
	int timeout = 0;
	ds18b20_sda=1;
	ds18b20_sda=0;
	ds18b20_delayus(80);
	ds18b20_sda=1;
	ds18b20_delayus(3);
	while(ds18b20_sda)
	{
		timeout++;
		if(timeout >TIME_OUT)
		{
			break;
		}
	}
	ds18b20_sda=1;
	ds18b20_delayus(70);
}

void ds18b20_write(unsigned char  cmd) 
{
	unsigned char  i;
	for(i=0;i<8;i++)
	{
		ds18b20_sda=1;
		ds18b20_sda=0;
		_nop_();
		cmd>>=1;
		ds18b20_sda=CY;
		ds18b20_delayus(10);
	} 
}

unsigned char  ds18b20_read()
{
	unsigned char  t=0,i,u=1;
	for(i=0;i<8;i++)
	{
		ds18b20_sda=1;
		ds18b20_sda=0;
		ds18b20_sda=1;
		_nop_();
		if(ds18b20_sda) 
		{ 
			t|=(u<<i);
			ds18b20_delayus(2);
			_nop_();
		}
		else
		{
			ds18b20_delayus(10);
		} 
	}
	return t; 
}

void ds18b20_start_get_temp()
{
	ds18b20_init();
	ds18b20_write(0xcc);
	ds18b20_write(0x44);

}

unsigned int  ds18b20_get_temp()
{
	unsigned char  low,high;
	unsigned int temp;
	float ftemp;
	ds18b20_init();
	ds18b20_write(0xcc);
	ds18b20_write(0xbe);
	low=ds18b20_read();
	high=ds18b20_read();
	temp = high;
	temp <<= 8;
	temp += low;
	ftemp = temp * 0.0625;
	temp = ftemp *10 + 0.5;
	
	return temp; 
}
 
 