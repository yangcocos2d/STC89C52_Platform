#ifndef _UART_H_
#define _UART_H_

#include <reg52.h>
void Uart_init();
void Uart_puts(const char* str);
char putchar(char c);
#endif