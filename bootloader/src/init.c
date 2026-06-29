
#include "init.h"

void setupBootPin() {
    // setup GPIO to check if boot pin is high, meaning bootloader is requested
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
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

void setupUART() {
    // Enable USART1 clock
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    //__HAL_RCC_AFIO_CLK_ENABLE(); already called in HAL_Init

    // Configure GPIO pins for USART1
    // PB6 = USART1_TX
    // PB7 = USART1_RX
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // needed since we remap UART to B6/7 instead of default A9/10
    __HAL_AFIO_REMAP_USART1_ENABLE();

    // Configure UART1
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }

    // Enable UART interrupt
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}