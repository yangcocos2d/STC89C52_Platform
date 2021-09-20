#include "AT24CXX.h"
#include "iic.h"

#define AT24CXX_IIC_SLAVE		0x50

void AT24CXX_write(char address,char *buffer,int length)
{
	int i=0;
	for(i=length;i>=0;i--)
	{
		buffer[i+1] = buffer[i];
	}
	buffer[0] = 	address;
	iic_write_data(AT24CXX_IIC_SLAVE, buffer, length+1);
}

void AT24CXX_read(char address,char *buffer,int length)
{
	int i=0;
	iic_random_read_data(AT24CXX_IIC_SLAVE,address, buffer, length);
}
