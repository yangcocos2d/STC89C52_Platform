#include "motor_encode.h"

sbit motor_in1 = P2^0;
sbit motor_in2 = P2^1;
sbit motor_en = P2^2;

void motor_encode_enable()
{
	motor_en = 1;
}

void motor_encode_disable()
{
	motor_en = 0;
}

void motor_encode_clockwise()
{
	motor_in1 = 1;
	motor_in2 = 0;

}

void motor_encode_anticlockwise()
{
	motor_in1 = 0;
	motor_in2 = 1;
}

void motor_encode_free_stop()
{
	motor_in1 = 0;
	motor_in2 = 0;
}

void motor_encode_faster_stop()
{
	motor_in1 = 1;
	motor_in2 = 1;
}