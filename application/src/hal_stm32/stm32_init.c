
#include "stm32_pwm.h"
#include "stm32_uart.h"
#include "stm32_init.h"
#include "stm32_gpio.h"
#include "stm32_hal.h"
#include "stm32f1xx_hal.h"

// Timer handles
//static TIM_HandleTypeDef htim3;

void boardInit() {
  // configures system tick
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  // for debugging execution times
  init_cycle_counter();

  // configure PWM timer and PWM pins, GPIO, encoder and ADC
  initGPIO();
  setupUART();
  setupADC(); // must be called before PWM init!
  setupPWMTimer();
  setupEncoder();
}

void enableInterrupts() {
  __enable_irq();
}

// setup encoder
void setupEncoder() {
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_DBGMCU_UNFREEZE_TIM3();
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // PA6: rising-edge interrupt input
  // PA7: direction input (read in ISR)
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*
  // TIM3 is not used as the encoder peripheral anymore; keep it clocked and reset.
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xFFFF;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
    Error_Handler();
  }
  __HAL_TIM_SET_COUNTER(&htim3, 0);
  HAL_TIM_Base_Start(&htim3);
  */

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_6) {
    encoderEdgeInterruptHandler();
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 1. Configure the Internal 8MHz Oscillator and the Multiplier Engine */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  
  /* Enable the PLL Multiplier */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2; // 8MHz / 2 = 4MHz entering PLL
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;            // 4MHz * 16 = 64MHz System Speed
  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 2. Route the 72MHz PLL Output to the Buses and Configure Dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK

                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // Switch Main clock to the 64MHz PLL output
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        // CPU Core runs at 64MHz (HCLK), max speed with internal oscillator
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;         // High-Speed Peripherals run at 64MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;         // CRUCIAL: APB1 bus max speed limit is 36MHz, so it runs at 32MHz

  /* FLASH_LATENCY_2 is mandatory when stepping past 48MHz up to 72MHz */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  // force update as suggested by CubeMX to avoid issues with SystemCoreClock not being updated
  SystemCoreClockUpdate();
  HAL_InitTick(TICK_INT_PRIORITY); // ensure systick is 1ms because we changed the clock cycle
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */