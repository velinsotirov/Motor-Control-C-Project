
#include <stdint.h>

#include "diag.h"
#include "stm32_hal.h"
#include "stm32f1xx_hal.h"

uint16_t getTimerVal() {
    // TODO: return the counter value using HAL functions
    return (uint16_t) TIMER_VAL;
}

uint32_t disableInterrupts() {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

// interrupt for calling tx uart, which also coutns for rx uart and controller execution
// HAL Callback called automatically from the STM32 ISR file
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    diagTrigger(); 
  }
}
