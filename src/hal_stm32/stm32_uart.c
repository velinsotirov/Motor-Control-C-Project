
#include <stdint.h>

#include "diag.h"
#include "ringbuffer.h"
#include "stm32_hal.h"
#include "stm32_init.h"
#include "stm32_uart.h"
#include "stm32f1xx_hal.h"

// UART handle for USART1
static UART_HandleTypeDef huart1;

void writeToUSART(uint8_t* data_out) {
  HAL_UART_Transmit(&huart1, data_out, 1, 100);
}

// enable and disable UART interrupts, abstracted for use by ASW
void enableTxInterrupt() {
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
}

void disableTxInterrupt() {
  __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
}

void setupUART() {
    // Enable USART1 clock
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure GPIO pins for USART1
    // PA9 = USART1_TX
    // PA10 = USART1_RX
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure UART1
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
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

// UART interrupt handler
void USART1_IRQHandler(void) {
    uint8_t data = 0;
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) {
        data = (uint8_t)(huart1.Instance->DR & 0xFF);
        ringbuffer_write(&rx_rb, data);
        __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
    }
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE)) {
        uint8_t oldest_byte = 0u;
        if (ringbuffer_read(&tx_rb, &oldest_byte, 1u)) {
            huart1.Instance->DR = (oldest_byte & 0xFF);
        } else {
            disableTxInterrupt();
        }
    }
}
