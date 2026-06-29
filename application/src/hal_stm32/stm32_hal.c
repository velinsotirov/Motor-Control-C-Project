
#include <stdint.h>

#include "diag.h"
#include "encoder.h"
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

void encoderEdgeInterruptHandler() {
  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET) {
    encoder_cnt++;
  } else {
    encoder_cnt--;
  }
}

// return encoder count and reset it
int16_t fetchAndResetEncoderCount() {
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  int16_t temp_cnt = encoder_cnt;
  encoder_cnt = 0;
  __set_PRIMASK(primask);
  return temp_cnt;
}

void setupWatchdog() {
  // TODO: implement!
}

void resetWatchdog() {
  // TODO: implement!
}
