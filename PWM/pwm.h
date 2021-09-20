#ifndef _PWM_H_
#define _PWM_H_
#include <reg52.h>

typedef struct  pwm
{
  int pwm_high_percentage;  
  int pwm_period;
  int pwm_counter;
}pwm;

int     pwm_is_high(pwm* my_pwm);
void    pwm_stepper(pwm* my_pwm);
#endif