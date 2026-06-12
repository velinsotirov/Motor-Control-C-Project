
#include <stdint.h>
#include <stdbool.h>

#include "current.h"
#include "stm32_adc.h"
#include "stm32_init.h"
#include "stm32_hal.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"

// ADC handle
ADC_HandleTypeDef hadc1;

void setupADC() {
    __HAL_RCC_ADC1_CLK_ENABLE();

    // ADC pinout
    // PA0  = ADC0
    // PA1  = ADC1
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ADC clock
    __HAL_RCC_ADC_CONFIG(RCC_CFGR_ADCPRE_DIV8); // 9MHz

    // ADC base config
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE; // only scan one input pin
    hadc1.Init.ContinuousConvMode = DISABLE; // one conversion per trigger, can be forced to sample multiple times for more accuracy
    hadc1.Init.DiscontinuousConvMode = DISABLE; // irrelevant
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // can be forced to trigger via HW for 0 latency but whatever
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 12bits are set to the right, first bits empty
    hadc1.Init.NbrOfConversion = 1; // has this something to do with ScanConvMode?
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    // ADC channel config
    ADC_ChannelConfTypeDef adcChannelConfig = {0};
    adcChannelConfig.Channel = ADC_CHANNEL_0;
    adcChannelConfig.Rank = ADC_REGULAR_RANK_1; // first in sequence
    adcChannelConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5; // with 9MHz ADC clock, this gives us 3.16us, similar to AVR's 3us
    if (HAL_ADC_ConfigChannel(&hadc1, &adcChannelConfig) != HAL_OK) {
        Error_Handler();
    }

    // calibration, first ADC measurement
    HAL_ADCEx_Calibration_Start(&hadc1);

    // NVIC
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 1, 0); // priority lower than TIM1, so timer can preempt ADC if needed
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}

// interrupt for triggering ADC lies within pwm file since htim1 is created there

// interrupt triggers when ADC conversion is complete and sends the value to the ASW
void ADC1_2_IRQHandler(void) {
    if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC)) { // why?
        currentADCticks = HAL_ADC_GetValue(&hadc1);
    }
}