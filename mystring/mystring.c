#include "mystring.h"

void str_itoa(char *buffer,int int_data)
{		
	int i=0;
	int temp = 0;
	int buffer_index = 0;
	int div =10000;

	if(int_data < 0)
	{
		buffer[0] = '-';
		int_data = -int_data;
	}
	
	
	for(i=0;i<5;i++)
	{
		if(i != 0)
		{
			temp = int_data%(div*10);
		}
		temp = temp/div;
		if(int_data>=div)
		{
			buffer[buffer_index] = temp + '0';
			buffer_index++;
		}	
		div = div/10;

	}
	
	if(int_data == 0)
	{
		buffer[0] = '0';
	}
		

}

void str_copy(char* des,char *src)
{	
	int i = 0; 
	while(src[i]!='\0')
	{
		des[i] = src[i];
		i++;
	}

}
