#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>

void pwmCount(void);

// 7 cycles * 62.5ns = 437.5ns (more than 400ns)
#define DELAY_0_4US() __asm__ __volatile__ ( \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
    "nop\n\t" \
)

#endif // PWM_H