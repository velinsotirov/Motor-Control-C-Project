#ifndef ATMEGA329P_PWM_H
#define ATMEGA329P_PWM_H

#include <stdint.h>
#include <stdbool.h>

#include "fixed_point.h"

void enable_pwm(void);
void disable_pwm(void);
void setupPWMTimer(void);
void set_duty_cycle(q8_8_t duty);

// 8 cycles * 62.5ns = 500ns
#define DELAY_0_5US() __asm__ __volatile__ ( \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
)

#endif // ATMEGA329P_PWM_H