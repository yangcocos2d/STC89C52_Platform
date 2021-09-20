#include "pcf8591.h"
#include "iic.h"

/*
要注意pcf8591的一个奇怪性质:当你写入ch时，它返回的却是上一次写入的ch的ADC值。
所以函数pcf8591_get_adc_value的功能是，指定要转换的通道ch，返回上一次转换通道的ADC值。
*/
unsigned char pcf8591_get_adc_value(unsigned char iic_slave,int ch)
{		
		unsigned char buffer[1];
 
		buffer[0] = ch;
		iic_write_data( iic_slave,buffer,1);
		buffer[0] = 0;
		iic_read_data( iic_slave,buffer,1);
		return buffer[0];
}

/*
依据手册，DAC转换时间为90us
*/
void pcf8591_set_dac_value(unsigned char iic_slave,unsigned char dac_data)
{		
		unsigned char buffer[2];

		buffer[0] = 1<<6;
		buffer[1] = dac_data;
		iic_write_data( iic_slave,buffer,2);
}	
