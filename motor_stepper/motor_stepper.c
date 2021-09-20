#include "motor_stepper.h"


#define MOTOR_STEPPER_PHASE 	4
/*
四相步进电机旋转脉冲(一周)
p3 p2 p1 p0
1  1  0  0 =>0x0C
0  1  1  0 =>0x06
0  0  1  1 =>0x03
1  0  0  1 =>0x09
*/
char motor_stepper_clockwise_code_list[MOTOR_STEPPER_PHASE] = {0x0c,0x06,0x03,0x09};
void motor_stepper_clockwise_once()
{
	static int cur_phase = MOTOR_STEPPER_PHASE - 1;
	PORT = motor_stepper_clockwise_code_list[cur_phase--];
	if(cur_phase < 0)
	{
		cur_phase = MOTOR_STEPPER_PHASE - 1;
	}
}

void motor_stepper_unclockwise_once()
{
	static int cur_phase = 0;
	PORT = motor_stepper_clockwise_code_list[cur_phase++];
	if(cur_phase >= MOTOR_STEPPER_PHASE)
	{
		cur_phase = 0;
	}
}

void motor_stepper_fast_stop()
{
	PORT = 0x00;
}

void motor_stepper_free()
{
	PORT = 0xff;
}