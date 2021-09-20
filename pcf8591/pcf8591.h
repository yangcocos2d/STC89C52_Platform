#ifndef _PCF8591_H_
#define _PCF8591_H_

#define PCF8591_SLAVE_ADDRESS_0 0x48
#define PCF8591_SLAVE_ADDRESS_1 0x49


unsigned char pcf8591_get_adc_value(unsigned char iic_slave,int ch);
void pcf8591_set_dac_value(unsigned char iic_slave,unsigned char dac_data);
#endif