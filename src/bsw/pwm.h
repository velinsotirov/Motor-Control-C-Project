
#include <stdint.h>
#include <stdbool.h>

// duty which is read by PWM interrupt
volatile uint8_t duty_compa = 0;

// pwm counter
volatile uint8_t pwmCounter = 0;

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