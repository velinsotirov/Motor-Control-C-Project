
#include "stm32_pwm.h"
#include "stm32_init.h"
#include "stm32f1xx_hal.h"

// Timer handles
static TIM_HandleTypeDef htim3;

void boardInit() {
  // configures system tick
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  // configure PWM timer and PWM pins, encoder and ADC
  setupPWMTimer();
  setupEncoder();
  setupADC();
}

// setup encoder
void setupEncoder() {
  // encoder pinout
  // PA6  = TIM3_CH1
  // PA7  = TIM3_CH2
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // encoder input
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Pull = GPIO_PULLUP; // encoder pulls to GND during a pulse
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // setup timer
  __HAL_RCC_TIM3_CLK_ENABLE();
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0; // count every edge
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xFFFF; // max 16bit value
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  // setup encoder
  TIM_Encoder_InitTypeDef encoderConfig = {0};
  encoderConfig.EncoderMode = TIM_ENCODERMODE_TI1; // count A only for simplicuty and so counter doesnt overflow as easily
  encoderConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  encoderConfig.IC2Polarity = TIM_ICPOLARITY_RISING; // count rising edges
  encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI; // why, what is this?
  encoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  encoderConfig.IC2Prescaler = TIM_ICPSC_DIV1; // what is this?
  encoderConfig.IC1Filter = 0x7;
  encoderConfig.IC2Filter = 0x7; // medium filter
  if (HAL_TIM_Encoder_Init(&htim3, &encoderConfig) != HAL_OK) {
    Error_Handler();
  }

  // start encoder
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2);
}

void setupWatchdog() {
  // TODO: implement!
}

void resetWatchdog() {
  // TODO: implement!
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.*/
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks*/
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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