
#include <stdint.h>
#include <stdbool.h>

#include "current.h"
#include "stm32_adc.h"
#include "stm32_init.h"
#include "stm32_hal.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"

// for profiling
static volatile uint32_t adcinterrupt_lastExecTicks = 0u;
static volatile uint32_t adc_execCounter = 0u;

// ADC handles
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_MultiModeTypeDef multimode;

void setupADC() {
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_ADC2_CLK_ENABLE();

    // ADC pinout
    // PA0  = ADC0, left shunt
    // PA1  = ADC1, right shunt
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ADC clock
    __HAL_RCC_ADC_CONFIG(RCC_CFGR_ADCPRE_DIV6); // 10.67MHz

    // ADC1  config
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE; // only scan one input pin
    hadc1.Init.ContinuousConvMode = DISABLE; // one conversion per trigger, can be forced to sample multiple times for more accuracy
    hadc1.Init.DiscontinuousConvMode = DISABLE; // irrelevant
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // can be forced to trigger via HW for 0 latency but whatever
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 12bits are set to the right, first bits empty
    hadc1.Init.NbrOfConversion = 1; // has this something to do with ScanConvMode?
    if (HAL_ADC_Init(&hadc1) != HAL_OK) { Error_Handler(); }

    // ADC2  config
    hadc2.Instance = ADC2;
    hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE; // only scan one input pin
    hadc2.Init.ContinuousConvMode = DISABLE; // one conversion per trigger, can be forced to sample multiple times for more accuracy
    hadc2.Init.DiscontinuousConvMode = DISABLE; // irrelevant
    hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1; // slave of adc1
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 12bits are set to the right, first bits empty
    hadc2.Init.NbrOfConversion = 1; // has this something to do with ScanConvMode?
    if (HAL_ADC_Init(&hadc2) != HAL_OK) { Error_Handler(); }

    // ADC1 channel config
    ADC_ChannelConfTypeDef adcChannelConfig = {0};
    adcChannelConfig.Channel = ADC_CHANNEL_0;
    adcChannelConfig.Rank = ADC_REGULAR_RANK_1; // first in sequence
    adcChannelConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5; // with 10.67MHz ADC clock, this gives us 2.67us, similar to AVR's 3us
    if (HAL_ADC_ConfigChannel(&hadc1, &adcChannelConfig) != HAL_OK) { Error_Handler(); }

    // ADC2 channel config
    adcChannelConfig.Channel = ADC_CHANNEL_1; // rest of the settings are the same
    if (HAL_ADC_ConfigChannel(&hadc2, &adcChannelConfig) != HAL_OK) { Error_Handler(); }

    // multimode config, ADC1 is master, ADC2 is slave
    multimode.Mode = ADC_DUALMODE_REGSIMULT;
    if ( HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) { Error_Handler(); }

    // calibration, first ADC measurement
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADCEx_Calibration_Start(&hadc2); // channel 2 should fire on its own

    // 3. CRUCIAL HAL FIX: Activate the Slave interface first natively using the pure HAL API!
    // This safely wakes up the slave core and primes it to wait for the master without executing a manual bit write
    if (HAL_ADC_Start(&hadc2) != HAL_OK) { Error_Handler(); }

    // NVIC
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 1, 0); // priority lower than TIM1, so timer can preempt ADC if needed
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
}

// interrupt for triggering ADC lies within pwm file since htim1 is created there

// interrupt triggers when ADC conversion is complete and sends the value to the ASW
void ADC1_2_IRQHandler(void) {
    adc_execCounter++;
    uint32_t start = get_cycles();
    if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC)) {

        /*
        // 1. Fetch your raw 12-bit motor current ticks
        uint32_t raw_adc_result = ADC1->DR;

        // unpack left and right ticks
        leftADCticks = (raw_adc_result & 0xFFFF); // ADC1 result, so PA0
        rightADCticks = (raw_adc_result >> 16);   // ADC2 result, so PA1
        */
        
        leftADCticks = HAL_ADC_GetValue(&hadc1);
        rightADCticks = HAL_ADC_GetValue(&hadc2);
        
        // 2. CRUCIAL: Clear the hardware flag so the interrupt stops firing!
        __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
    }
    uint32_t time =  get_cycles() - start;
    if (time > adcinterrupt_lastExecTicks) { adcinterrupt_lastExecTicks = time; } // for profiling
}