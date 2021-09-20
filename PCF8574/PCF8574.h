#ifndef _PCF8574_H_
#define _PCF8574_H_


void 					PCF8574_write_port(char chip_id,unsigned char port_data);
unsigned char PCF8574_read_port(char chip_id);
void 					PCF8574_set_bit(char chip_id,char bit_num,char bit_set);
unsigned char PCF8574_read_bit(char chip_id,char bit_num);
#endif