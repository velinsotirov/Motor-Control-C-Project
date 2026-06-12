#ifndef STM32_ADC_H
#define STM32_ADC_H

#include "stm32f1xx_hal.h"

void setupADC(void);

extern ADC_HandleTypeDef hadc1;

#endif // STM32_ADC_H