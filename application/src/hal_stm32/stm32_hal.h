#ifndef STM32_HAL_H
#define STM32_HAL_H

#include <stdint.h>

#include "stm32f1xx_hal.h"

uint32_t getTimerVal(void);
uint32_t disableInterrupts(void);
void encoderEdgeInterruptHandler(void);
int16_t fetchAndResetEncoderCount(void);
void setupWatchdog(void);
void resetWatchdog(void);

inline void init_cycle_counter(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable Trace system
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;           // Enable Cycle Counter
}

inline uint32_t get_cycles(void) {
    return DWT->CYCCNT;
}

#define RESTORE_SREG(sreg) __set_PRIMASK(sreg)

#define TIMER_VAL TIM2->CNT

#endif // STM32_HAL_H