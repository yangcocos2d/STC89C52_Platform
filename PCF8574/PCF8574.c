
#include "PCF8574.h"
#include "iic.h"

#define PCF8574_IIC_SLAVE_BASE 0x20

void PCF8574_write_port(char chip_id,unsigned char port_data)
{
	unsigned char buffer[1];
	buffer[0] = 	port_data;
	iic_write_data(chip_id + PCF8574_IIC_SLAVE_BASE, buffer, 1);
}

unsigned char PCF8574_read_port(char chip_id)
{
	unsigned char buffer[1];
	iic_read_data(chip_id + PCF8574_IIC_SLAVE_BASE, buffer, 1);
	return buffer[0];
}

void PCF8574_set_bit(char chip_id,char bit_num,char bit_set)
{
	unsigned char port_data = PCF8574_read_port(chip_id);
	if(bit_set!=0)
	{
		port_data |= (1<<bit_num);
	}
	else
	{
		port_data &= ~(1<<bit_num);
	}
	PCF8574_write_port(chip_id,port_data);
	
}

unsigned char PCF8574_read_bit(char chip_id,char bit_num)
{
	unsigned char port_data = PCF8574_read_port(chip_id);
	port_data &= (1<<bit_num);
	if(port_data == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
 
}