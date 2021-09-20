#include "UART.h"

//-----------------UART---------------
void Uart_init()
{
    TMOD |= 0x20; 
	  TH1 = 0xfd; 
    TL1 = 0xfd;
		SM0 = 0;
		SM1 = 1;
		REN = 1;
    PCON = 0x00; 
   
    ES = 1; 
    EA = 1; 
    TR1 = 1; 	

}

void Uart_Send( char Data)
{
	SBUF=Data; 
	while(TI==0);			 
	TI=0;
 
}

char putchar(char c)
{
	
	EA = 0;
	Uart_Send(c);
	EA = 1;
	return c;
	
}

void Uart_puts(const char* str)
{
	int i = 0;
 
	
	while(str[i]!='\0')
	{
		putchar(str[i]);
		i++;
	}
 
}

void Uart_IRQ() interrupt 4
{
	//串口数据接收标志位
 	if(RI == 1)
	{
		RI = 0;
		//data = SBUF
	
	}		
	//串口数据发送完成标志位
	if(TI == 0)
	{
		TI = 0;
		
	}
	
}
