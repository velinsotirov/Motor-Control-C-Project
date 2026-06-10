#ifndef STM32_UART_H
#define STM32_UART_H

#include <stdint.h>
#include <stdbool.h>

#include "ringbuffer.h"

void uart_init(void);

void writeToUSART(uint8_t*);

void enableTxInterrupt(void);
void disableTxInterrupt(void);

#endif // STM32_UART_H