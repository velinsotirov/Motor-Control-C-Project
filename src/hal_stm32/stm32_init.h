#ifndef STM32_INIT_H
#define STM32_INIT_H

#include "stm32f1xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void setupEncoder(void);
void boardInit(void); 
void Error_Handler(void);
void SystemClock_Config(void);
void setupWatchdog(void);
void resetWatchdog(void);
void setupADC(void);

#endif /* STM32_INIT_H */