#include <STC15F2K60S2.H>
#include <intrins.h>
#define uchar unsigned char 	
	
sbit RS=P3^2;
sbit RW=P3^3;
sbit E=P3^4;
sbit T_RST = P3^5;
sbit T_CLK = P3^6;
sbit T_IO = P3^7;

uchar datechar[] = {"DATE:"};
uchar timechar[] = {"TIME:"};
uchar datebuffer[10]={0x32,0x30,0,0,0x2d,0,0,0x2d,0,0};
uchar timebuffer[8]={0,0,0x3a,0,0,0x3a,0,0};
uchar weekbuffer={0x30};

void writeB(uchar dat)
{
	uchar i;
	for(i=8;i>0;i--)
	{
		T_IO=dat&0X01;
		T_CLK=1;
		T_CLK=0;
		dat=dat>>1;
	}
}
uchar ReadB(void)
{
	uchar i,readdat=0;
	for(i=8;i>0;i--)
	{
		readdat=readdat>>1;
		if(T_IO)
		{
			readdat|=0x80;
		}
		T_CLK=1;
		T_CLK=0;
	}
	return(readdat);
}
void W1302(uchar address,uchar dat)
{
	T_RST=0;
	T_CLK=0;
	_nop_();
	_nop_();
	T_RST=1;
  _nop_();
	_nop_();
	writeB(address);
	writeB(dat);
	T_CLK=1;
	T_RST=0;
}

uchar R1302(uchar address)
{
	uchar dat=0;
	T_RST=0;
	T_CLK=0;
	T_RST=1;
	writeB(address);
	dat=ReadB();
	T_CLK=1;
	T_RST=0;
  return(dat);		
}

void delay(int n)
{
	int i=0,j=0;
	for(i=0;i<=n;n++);
	{
	for(j=0;j<=200;j++);
	}
}
//写数据函数
void writedat (uchar dat)
{
	RS=1;
	RW=0;
	E=0;
	P2=dat;
	delay(5);
	E=1;
	E=0;
  
}

void writecom (uchar com)
{
	RS=0;
	RW=0;
	E=0;
	P2=com;
	delay(5); 
	E=1;
	E=0;
}
void initlcd()
{
	writecom(0x38);
  writecom(0x0c);
	writecom(0x06);
	writecom(0x01);
}

void display()
{
	int i=0,temp=0;
//	年
	temp=R1302(0X8D);
	datebuffer[2]='2';
	datebuffer[3]='0';
//	星期
	temp=R1302(0X8b);
	weekbuffer='1';
//	月份
	temp=R1302(0X89);
	datebuffer[5]=0x30+temp/16;
	datebuffer[6]=0x30+temp%16;
//	日
	temp=R1302(0X87);
	datebuffer[8]=0x30+temp/16;
	datebuffer[9]=0x30+temp%16;
//	小时
	temp=R1302(0X85);
	temp=temp&0x7f;
	timebuffer[0]=0x30+temp/16;
	timebuffer[1]=0x30+temp%16;
//	分钟
	temp=R1302(0X83);
	temp=temp&0x7f;
	timebuffer[3]=0x30+temp/16;
	timebuffer[4]=0x30+temp%16;
//	秒
	temp=R1302(0X81);
	temp=temp&0x7f;
	timebuffer[6]=0x30+temp/16;
	timebuffer[7]=0x30+temp%16;
//	lcd 第一行
	writecom(0x80);
	for(i=0;i<5;i++)
	{
		writedat (datechar[i]);
	}
//	lcd 第二行
	writecom(0xc0);
	for(i=0;i<5;i++)
	{
		writedat(timechar[i]);
	}
//	显示日历
	writecom (0x86);
	for(i=0;i<10;i++)
	{
		writedat(datebuffer[i]);
	}
	writecom(0xc6);
	for(i=0;i<8;i++)
	{
		writedat(timebuffer[i]);
	}
	writedat(0x11);
	writedat(weekbuffer);
}
void main()
{
	initlcd();
W1302(0x8e,0x00);	
//	写入年
	W1302(0x8c,0x20);
//	写入星期
	W1302(0x8a,0x01);
//	写入月
	W1302(0x88,0x03);
//	写入日
	W1302(0x86,0x27);
//	写入时
	W1302(0x84,0x12);
//	写入分
	W1302(0x82,0x11);
//	写入秒
	W1302(0x80,0x05);
	
	W1302(0x8e,0x80);
  while(1)
  {
	  display();
  }
}