#ifndef _IIC_H_
#define _IIC_H_

#include <reg52.h>

sbit SCL = P3^4;
sbit SDA = P3^5;


int iic_read_data(char iic_address,char *buffer,int length);
int iic_write_data(char iic_address,char *buffer,int length);
int iic_random_read_data(char iic_address,char data_address,char *buffer,int length);

#endif