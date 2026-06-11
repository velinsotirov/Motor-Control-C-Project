
#include "stm32_pwm.h"
#include "stm32_init.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_tim.h"

// Timer handle for PWM
static TIM_HandleTypeDef htim1;

// enable PWM via main output enable
void enable_pwm() {
    TIM1->BDTR |= TIM_BDTR_MOE;
}

// disable PWM via main output enable
// this automatically switches pins to idle state config (HS floating, LS closed)
void disable_pwm() {
    __HAL_TIM_DISABLE(htim1);
}

void set_duty_cycle(int16_t duty) {
  // is duty positive
  uint16_t duty_compa;
  if (duty >= 0) {
    duty_compa = duty;
    TIM1->CCR1 = duty_compa * 18; // we count to 1800, so 100% duty is 1800
    TIM1->CCR2 = 0u;
  }
  else {
    duty_compa = -duty;
    TIM1->CCR1 = 0u;
    TIM1->CCR2 = duty_compa * 18; // we count to 1800, so 100% duty is 1800
  }
}

void setupPWMTimer() {
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // PWM pinout
    // PA8  = TIM1_CH1  (high side drive)
    // PB13 = TIM1_CH1N (low side drive, complementary)
    // PA9  = TIM1_CH2  (high side drive)
    // PB14 = TIM1_CH2N (low side drive, complementary)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // push pull alternate function
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // fastest pin toggling, recommendeded for fast PWM

    // LB HS
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // LB LS
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // RB HS
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // RB LS
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // timer config
    // 72MHz, prescaler 0 (divide by 1), count to 1800 then down
    // -> 72MHz / 1800 / 2 = 20kHz
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0; // 72Mhz / (0+1)
    htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
    htim1.Init.Period = 1800;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // not needed, as PWM frequency is always constant
    htim1.Init.RepetitionCounter = 0; // fires every (1 + RepetitionCounter) interrupts, we wanna fire every time so we set to 0
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }

    // channel 1 config, meaning LB HS/LS
    TIM_OC_InitTypeDef channelConfig;
    channelConfig.Pulse = 0; // duty cycle
    channelConfig.OCMode = TIM_OCMODE_PWM1; // outputs high when cnt < compare, so low side ON period is in the middle
    channelConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    channelConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    channelConfig.OCFastMode = TIM_OCFAST_DISABLE; // not needed
    channelConfig.OCIdleState = TIM_OCIDLESTATE_RESET; // HS is low when idle
    channelConfig.OCNIdleState = TIM_OCIDLESTATE_SET; // LS is closed when idle (to charge bootstrap)
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &channelConfig, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }

    // channel 2 config, meaning RB HS/LS, is identical
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &channelConfig, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    // dead time config
    TIM_BreakDeadTimeConfigTypeDef deadtimeConfig = {0};
    deadtimeConfig.OffStateIDLEMode = TIM_OSSR_ENABLE; // do we set pin states to idle when channel is disabled
    deadtimeConfig.OffStateIdleMode = TIM_OSSI_ENABLE; // do we set pin states to idle when timer is disabled
    deadtimeConfig.LockLevel = TIM_LOCKLEVEL_1; // lock safety critical registers after first write
    deadtimeConfig.DeadTime = 36; // 500ns with 72MHz clock
    deadtimeConfig.BreakState = TIM_BREAK_ENABLE; // trigger break state when a pin changes state unexpectedly
    deadtimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW; // wunexpected state change is HIGH to LOW
    deadtimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE; // once BRK state goes away, stay disabled
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &deadtimeConfig) != HAL_OK) {
        Error_Handler();
    }

    // ADC starts sampling around middle of PWM period, and with a freq of 14MHz and prescaler 8, takes 1.5 cycles to sample,
    // which is 14MHz/8 = 1.75MHz, so 1.5 periods is 0.857us. this limits max duty to 94%
    TIM_OC_InitTypeDef channelConfig;
    channelConfig.Pulse = 1692;
    channelConfig.OCMode = TIM_OCMODE_TIMING;
    channelConfig.OCPolarity = TIM_OCPOLARITY_HIGH; // irrelevant since we arent toggling a pin with this channel
    channelConfig.OCFastMode = TIM_OCFAST_DISABLE; // same as above
    TIM_OC1_SetConfig(&htim1, CCstruct)
    if (HAL_TIM_PWM_ConigChannel(&htim1, &channelConfig, TIM_CHANNEL_3) != HAL_OK) {
        Error_Handler();
    }

    // Enable NVIC interrupt channel
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0); // capture and compare
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
    HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 0, 0); // capture and compare
    HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn); // break input

    // enable interrupts
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC3);

    // enable pwm
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

    // Start Timer base with Interrupt generation enabled
    HAL_TIM_Base_Start_IT(&htim1);
}

// cc interrupt which starts ADC conversion
void TIM1_CC_IRQHandler(void) {
    // fires for eevry channel, so we need to check if channel 3 triggered AND if we're upcounting
    // dir = 1 is downcounting, so we wanna execute when dir = 0
    if(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_CC3) && !(TIM1->CR1 & TIM_CR1_DIR)) {
        __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
        HAL_ADC_Start_IT(&hadc1);
    }
}

// break interrupt
void TIM1_BRK_IRQHandler(void) {
    // hw has already driven us to safe state, here we only update a flag to notify the SW
}
