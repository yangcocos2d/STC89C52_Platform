#ifndef _DS18B20_H_
#define _DS18B20_H_
#include <reg52.h>

sbit ds18b20_sda = P1^5;
void ds18b20_start_get_temp();
unsigned int  ds18b20_get_temp();
#endif