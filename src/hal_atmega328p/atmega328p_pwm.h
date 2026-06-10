#ifndef ATMEGA329P_PWM_H
#define ATMEGA329P_PWM_H

#include <stdint.h>
#include <stdbool.h>

void pwmCount(void);

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