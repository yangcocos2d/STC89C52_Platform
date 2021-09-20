#ifndef _AT24CXX_H_
#define _AT24CXX_H_

#include <reg52.h>

void AT24CXX_delayms(unsigned int ms);
void AT24CXX_read(char address,char *buffer,int length);
void AT24CXX_write(char address,char *buffer,int length);
#endif