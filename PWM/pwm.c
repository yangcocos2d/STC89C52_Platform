#include "pwm.h"


int pwm_is_high(pwm* my_pwm)
{
    if(my_pwm->pwm_counter >=  (my_pwm->pwm_high_percentage * my_pwm->pwm_period / 100))
    {
			 
        return 0;
    }
    else
    {
        return 1;
    }
}

void pwm_stepper(pwm* my_pwm)
{
    my_pwm->pwm_counter++;
    if(my_pwm->pwm_counter >= my_pwm->pwm_period)
    {
        my_pwm->pwm_counter = 0;
    }

}