#ifndef _BUTTON_MATRIX_H_
#define _BUTTON_MATRIX_H_
#include <reg52.h>

#define KEY_PORT 	P1


unsigned char button_matrix_scan();
unsigned char button_matrix_scan_wait_free();
#endif