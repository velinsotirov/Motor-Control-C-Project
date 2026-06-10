#ifndef STM32_HAL_H
#define STM32_HAL_H

#include <stdint.h>

uint16_t getTimerVal(void);

uint32_t disableInterrupts(void);

#define RESTORE_SREG(sreg) __set_PRIMASK(sreg)

#define TIMER_VAL TIM2->CNT

#endif /* STM32_HAL_H */