#include "pcf8591.h"
#include "iic.h"

/*
Ҫע��pcf8591��һ���������:����д��chʱ�������ص�ȴ����һ��д���ch��ADCֵ��
���Ժ���pcf8591_get_adc_value�Ĺ����ǣ�ָ��Ҫת����ͨ��ch��������һ��ת��ͨ����ADCֵ��
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
�����ֲᣬDACת��ʱ��Ϊ90us
*/
void pcf8591_set_dac_value(unsigned char iic_slave,unsigned char dac_data)
{		
		unsigned char buffer[2];

		buffer[0] = 1<<6;
		buffer[1] = dac_data;
		iic_write_data( iic_slave,buffer,2);
}	
