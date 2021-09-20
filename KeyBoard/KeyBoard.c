#include "KeyBoard.h"
#include "PCF8574.h"

/*
���̵����궨��
o-------------------->x(��0��ʼ)
|
|
|
v
y
(��0��ʼ)
*/

unsigned char KeyBoard_get_status(int x,int y)
{
	unsigned char is_key_press = 1;
	PCF8574_write_port(0,0xFF);
	PCF8574_write_port(1,0xFF);
	PCF8574_set_bit(0,x,0);
	PCF8574_set_bit(1,y,1);
	is_key_press = PCF8574_read_bit(1,y);
	return !is_key_press;
}

unsigned char KeyBoard_is_loosen(KEY_BLOCK *key)
{
	unsigned char key_is_loosen = 0;
	key->cur_status =  KeyBoard_get_status(key->x,key->y);
	if(key->last_status == 1 && key->cur_status == 0)
	{
		key_is_loosen = key->Key_Code;
	}
	key->last_status = key->cur_status;
	return key_is_loosen;
}