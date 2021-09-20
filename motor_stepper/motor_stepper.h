#ifndef _MOTOR_STEPPER_H_
#define _MOTOR_STEPPER_H_

#include <reg52.h>
#define PORT P2			//低位输出四相脉冲

void motor_stepper_clockwise_once();
void motor_stepper_unclockwise_once();
void motor_stepper_fast_stop();
void motor_stepper_free();

#endif