#ifndef STM32_PWM_H
#define STM32_PWM_H

#include <stdint.h>

#include "fixed_point.h"

void setupPWMTimer(void);
void enable_pwm(void);
void disable_pwm(void);
void set_duty_cycle(q8_8_t);

#endif // STM32_PWM_H