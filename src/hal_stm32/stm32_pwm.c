
#include <stdint.h>
#include <stdbool.h>

#include "stm32_hal.h"
#include "stm32_pwm.h"
#include "stm32_adc.h"
#include "stm32_init.h"
#include "stm32f1xx_hal.h"
#include "fixed_point.h"

// Timer handle for PWM
static TIM_HandleTypeDef htim1;

static volatile bool upcounting = true;

// for profiling
static volatile uint32_t tim1interrupt_lastExecTicks = 0u;
static volatile uint32_t tim1_execCounter = 0u;

static void force_pwm_outputs_off(void) {
    /*
     * Stop the timer and explicitly disable the output channels so the next enable
     * can start from a known, clean state. This is important because the HAL PWM
     * start/stop routines track channel state internally and re-enable must see the
     * channels in READY state.
     */
    __HAL_TIM_DISABLE(&htim1);
    htim1.Instance->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE | TIM_CCER_CC2E | TIM_CCER_CC2NE);
    __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim1);
    htim1.Instance->CCR1 = 0u;
    htim1.Instance->CCR2 = 0u;
    __HAL_TIM_SET_COUNTER(&htim1, 0u);

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

// enable PWM via main output enable
void enable_pwm() {
    force_pwm_outputs_off();

    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    __HAL_TIM_MOE_ENABLE(&htim1);
    __HAL_TIM_ENABLE(&htim1);
}

// disable PWM via main output enable
// this forces the bridge to its safe inactive state without changing pin configuration
void disable_pwm() {
    force_pwm_outputs_off();
}

void set_duty_cycle(q8_8_t duty) {
  // is duty positive
  uint16_t duty_abs;
  if (duty >= 0) {
    duty_abs = (uint16_t) duty;
    // we count to 1600, so 100% duty is 1600
    // we receive duty as q8_8, so its 256 times larger than the actual duty %
    // we count to 1600, which is 16x larger than 100
    // therefore we must divide by 16 to get the proper count value
    htim1.Instance->CCR1 = (uint16_t) (duty_abs / 16);
    htim1.Instance->CCR2 = 0u;
  }
  else {
    duty_abs = (uint16_t) (-duty);
    htim1.Instance->CCR1 = 0u;
    htim1.Instance->CCR2 = (uint16_t) (duty_abs / 16);
  }
}

void setupPWMTimer() {
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    //__HAL_RCC_GPIOB_CLK_ENABLE(); already called in setupUART

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
    // 64MHz, prescaler 0 (divide by 1), count to 1600 then down
    // -> 64MHz / 1600 / 2 = 20kHz
    htim1.Instance = TIM1;
    // TODO: remove once testing is completed!
    htim1.Init.Prescaler = 19999; // 64Mhz / (0+1) // 20000 for testing so we can see an LED with PWM!
    htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
    htim1.Init.Period = 1600; // used to be 1800 with 72MHz
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // not needed, as PWM frequency is always constant
    htim1.Init.RepetitionCounter = 0; // fires every (1 + RepetitionCounter) interrupts, we wanna fire every time so we set to 0
    
    // initialize PWM timer instead of normal, since with normal timer,
    // activating the complementary PIN causes a hard fault
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
    deadtimeConfig.OffStateRunMode = TIM_OSSR_ENABLE; // force the configured idle state when the timer stops
    deadtimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE; // force the configured idle state when the MCU enters idle
    deadtimeConfig.LockLevel = TIM_LOCKLEVEL_1; // lock safety critical registers after first write
    deadtimeConfig.DeadTime = 32; // 500ns with 64MHz clock
    // TODO: switch to ENABLE once motor is connected
    deadtimeConfig.BreakState = TIM_BREAK_DISABLE; // trigger break state when a pin changes state unexpectedly
    deadtimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW; // wunexpected state change is HIGH to LOW
    deadtimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE; // once BRK state goes away, stay disabled
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &deadtimeConfig) != HAL_OK) {
        Error_Handler();
    }

    // ADC starts sampling around middle of PWM period, and with a freq of 14MHz and prescaler 8, takes 1.5 cycles to sample,
    // which is 14MHz/8 = 1.75MHz, so 1.5 periods is 0.857us. this limits max duty to 94%
    // TODO: check if going to 64MHz changed anything!
    TIM_OC_InitTypeDef channelConfig2;
    channelConfig2.Pulse = 1504; // used to be 1692 with 72MHz
    channelConfig2.OCMode = TIM_OCMODE_TIMING;
    channelConfig2.OCPolarity = TIM_OCPOLARITY_HIGH; // irrelevant since we arent toggling a pin with this channel
    channelConfig2.OCFastMode = TIM_OCFAST_DISABLE; // same as above
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &channelConfig2, TIM_CHANNEL_3) != HAL_OK) {
        Error_Handler();
    }

    // Enable NVIC interrupt channel
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0); // capture and compare
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);

    HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 0, 0); // break input
    HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);

    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0); // roll over
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    // enable interrupts
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC3);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);

    // leave outputs disabled until the controller explicitly enables the powerstage
    force_pwm_outputs_off();
}

// cc interrupt which starts ADC conversion
void TIM1_CC_IRQHandler(void) {
    tim1_execCounter++;
    uint32_t start = get_cycles(); // for profiling

    //bool upcounting = !(htim1.Instance->CR1 & TIM_CR1_DIR);

    // fires for every channel, so we need to check if channel 3 triggered
    if(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_CC3)) {
        // 1. Clear the hardware timer flag so it doesn't fire again immediately!
        __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_CC3); 
        
        // we are upcounting
        if (upcounting) {
            // 2. Clear ADC flags and launch sampling
            __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
            HAL_ADC_Start_IT(&hadc1);

            upcounting = false;
        }
    }

    uint32_t time =  get_cycles() - start;
    if (time > tim1interrupt_lastExecTicks) { tim1interrupt_lastExecTicks = time; } // for profiling
}

// Define the Update interrupt handler to catch when the counter completes a cycle
void TIM1_UP_TIM10_IRQHandler(void) {
    // Check if the Update Interrupt Flag caused the event
    if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE)) {
        // 1. CRUCIAL: Instantly clear the flag so it doesn't trigger an interrupt storm!
        __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
        
        // 2. Set our tracker flag to TRUE because the timer has hit 0 and is starting to count UP
        upcounting = true;
    }
}

// break interrupt
void TIM1_BRK_IRQHandler(void) {
    // hw has already driven us to safe state, here we only update a flag to notify the SW
    // TODO: add this flag!
}
