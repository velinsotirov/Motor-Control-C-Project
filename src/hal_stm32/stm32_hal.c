
#include <stdint.h>

#include "diag.h"
#include "stm32_hal.h"
#include "stm32f1xx_hal.h"

uint32_t getTimerVal() {
    return HAL_GetTick();
}

uint32_t disableInterrupts() {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

// return encoder count and reset it
int16_t fetchAndResetEncoderCount() {
  int16_t temp_cnt = (int16_t) TIM3->CNT;
  TIM3->CNT = 0;
  return temp_cnt;
}
