
#include <stdint.h>
#include <stdbool.h>

#include "init.h"
#include "stm32f1xx_hal.h"

// bootloader is 12kB
#define BOOTLOADER_SIZE (0x3000UL)
#define APP_START_ADDRESS (FLASH_BASE + BOOTLOADER_SIZE)

static void jumpToApp(void);

int main() {
    HAL_Init();
    SystemClock_Config();
    setupBootPin();

    // check if pin is high
    bool bootloaderRequested = (bool) (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET);

    // jump to main app if bootloader not requested
    if (!bootloaderRequested) {
        jumpToApp();
    }
    // if bootloader requested, setup UART so we can receive the data
    else {
        setupUART();

        
    }

    return 0;
}

static void jumpToApp() {
    typedef void (*voidFcn)(void);

    uint32_t* reset_vector_entry = (uint32_t*) (APP_START_ADDRESS + 4UL);
    uint32_t* reset_vector_address = (uint32_t*) (*reset_vector_entry);
    voidFcn jumpToAppFunction = (voidFcn) reset_vector_address;
    jumpToAppFunction();
}
