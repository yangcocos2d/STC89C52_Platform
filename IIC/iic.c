#include "iic.h"
#include <intrins.h>
#define TIME_OUT  100
 
#define iic_delay_5us()   {}//{_nop_();_nop_();_nop_();_nop_();_nop_();}

void iic_start()
{
	SCL = 1;
	SDA = 1;
	iic_delay_5us( );
	SDA = 0;
	iic_delay_5us( );
	SCL = 0;
	SDA = 1;
}

void iic_stop()
{
	SCL = 0;
	SDA = 0;
	SCL = 1;
	iic_delay_5us( );
	SDA = 1;
	iic_delay_5us( );
	SDA = 0;
	SDA = 1;
}

void iic_sendbit(char bit_data)
{
	SCL = 0;
	SDA = bit_data;
	iic_delay_5us( );
	SCL = 1;
	iic_delay_5us( );
	SCL = 0;
	SDA = 1;
}

bit iic_readbit()
{
	bit ret = 0;
	SCL = 0;
	SDA = 1;
	iic_delay_5us( );
	SCL = 1;
	iic_delay_5us( );
	ret = SDA;
	SCL = 0;
	SDA = 1;
	return ret;
}

void iic_write_byte(unsigned char writedata)
{	
	int i;
 
	for(i=0;i<8;i++)
	{
		if((writedata&0x80)!=0)
		{
			iic_sendbit(1);
		}
		else
		{
			iic_sendbit(0);
		}
		writedata<<=1;
	}
	
}

char iic_read_byte( )
{	
	int i;
	char readdata = 0;
	char readdata_bit = 0;
	for(i=7;i>=0;i--)
	{
		readdata_bit = iic_readbit();
		readdata|= (readdata_bit << i);
	}
	return readdata;
}

char iic_waitack()
{
 
	int timeout = 0;
	while(1 ==iic_readbit())
	{
		timeout++;
		if(timeout >  TIME_OUT)
		{
			return 0;
		}
	}
 
	return 1;
}

int iic_read_data(char iic_address,char *buffer,int length)
{
	int i=0;
	iic_start();
	iic_address<<=1;
	iic_address|=0x01;
	iic_write_byte(iic_address);
	if(0 == iic_waitack())
	{
		return 0;
	}
 
	
	for(i=0;i<length;i++)
	{
	  buffer[i] = iic_read_byte();
		if(i < length - 1)
		{
			iic_sendbit(0);
		}
		else
		{
			iic_sendbit(1);
		}
	
	}
 
	iic_stop();
 
	return 1;
}

int iic_random_read_data(char iic_address,char data_address,char *buffer,int length)
{
	int i=0;
	char iic_address_merge_rw;
	iic_start();
	iic_address_merge_rw = iic_address<<1;
	iic_address_merge_rw &= ~0x01;
	iic_write_byte(iic_address_merge_rw);
	if(0 == iic_waitack())
	{
		return 0;
	}
	
	iic_write_byte(data_address);
	if(0 == iic_waitack())
	{
		return 0;
	}
 
	iic_start();
	iic_address_merge_rw = iic_address<<1;
	iic_address_merge_rw |= 0x01;
	iic_write_byte(iic_address_merge_rw);
	if(0 == iic_waitack())
	{
		return 0;
	}
	
	for(i=0;i<length;i++)
	{
	  buffer[i] = iic_read_byte();
		if(i < length - 1)
		{
			iic_sendbit(0);
		}
		else
		{
			iic_sendbit(1);
		}
	
	}
 
	iic_stop(); 
	
	return 1;
}

int iic_write_data(char iic_address,char *buffer,int length)
{
	int i=0;
	
	iic_start();
	iic_address<<=1;
	iic_address&=~(0x01);
	iic_write_byte(iic_address);
 
	if(0 ==iic_waitack())
	{
		return 0;
	}
	 
 
	for(i=0;i<length;i++)
	{
	  iic_write_byte(buffer[i]);
		if(0 ==iic_waitack())
		{
			return 0;
		}
	}
	
	iic_stop();
	
	return 1;
}