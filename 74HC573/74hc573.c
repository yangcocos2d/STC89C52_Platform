#include "74hc573.h"

void 	 locker_unlock(int locker_sel)
{
		if(locker_sel == 0)
		{
			LOCKER_LE_0 = 1;
		}
		if(locker_sel == 1)
		{
			LOCKER_LE_1 = 1;
		}
}

void   locker_lock()
{
		LOCKER_LE_0 = 0;
		LOCKER_LE_1 = 0;
}

void   locker_datset(int locker_sel,unsigned char dat)
{
		locker_lock();
		locker_unlock(locker_sel);
		LOCKER_DATA = dat;
		locker_lock();	
}

void   locker_setbit(int locker_sel,int bit_set)
{		locker_lock();
		locker_unlock(locker_sel);
		LOCKER_DATA |= (1<<bit_set);
		locker_lock();
}

void   locker_resetbit(int locker_sel,int bit_set)
{		locker_lock();
		locker_unlock(locker_sel);
		LOCKER_DATA &= ~(1<<bit_set);
		locker_lock();
}