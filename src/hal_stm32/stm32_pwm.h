#ifndef STM32_PWM_H
#define STM32_PWM_H

#include <stdint.h>

void setupPWMTimer(void);
void enable_pwm(void);
void disable_pwm(void);
void set_duty_cycle(int16_t);

#endif // STM32_PWM_H