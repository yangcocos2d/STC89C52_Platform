#ifndef _MOTOR_ENCODE_H_
#define _MOTOR_ENCODE_H_

#include <reg52.h>

void motor_encode_enable();
void motor_encode_disable();
void motor_encode_clockwise();
void motor_encode_anticlockwise();
void motor_encode_faster_stop();
void motor_encode_free_stop();
#endif