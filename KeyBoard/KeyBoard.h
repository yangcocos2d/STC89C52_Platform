#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

typedef struct KEY_BLOCK
{
	int x;
	int y;
	char Key_Code;
	unsigned char last_status;
	unsigned char cur_status;
}KEY_BLOCK;

unsigned char KeyBoard_get_status(int x,int y);
unsigned char KeyBoard_is_loosen(KEY_BLOCK *key);
#endif