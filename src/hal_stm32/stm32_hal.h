#ifndef STM32_HAL_H
#define STM32_HAL_H

#include <stdint.h>

#include "stm32f1xx_hal.h"

uint32_t getTimerVal(void);

uint32_t disableInterrupts(void);

#define RESTORE_SREG(sreg) __set_PRIMASK(sreg)

#define TIMER_VAL TIM2->CNT

#endif /* STM32_HAL_H */