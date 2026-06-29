
#include "stm32_gpio.h"
#include "stm32f1xx_hal.h"

void initGPIO() {
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void toggleLED() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}
