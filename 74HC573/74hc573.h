#ifndef _74HC573_H_
#define _74HC573_H_

#include <reg52.h>


sbit 		LOCKER_LE_0 = P2^0;
sbit 		LOCKER_LE_1 = P2^1;
#define 	LOCKER_DATA 	P3

#define LOCKER_0	0
#define LOCKER_1	1

void   locker_datset(int locker_sel,unsigned char dat);
void   locker_setbit(int locker_sel,int bit_set);
void   locker_resetbit(int locker_sel,int bit_set);

#endif