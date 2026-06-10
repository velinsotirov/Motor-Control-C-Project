
#include <stdint.h>
#include <stdbool.h>

#include "diag.h"
#include "system.h"
#include "global.h"
#include "encoder.h"
#include "controller.h"

#ifdef __AVR_ATmega328P__
#include "atmega328p_init.h"
#include "atmega328p_hal.h"
#else
#include "stm32_init.h"
#include "stm32_hal.h"
#endif

// last execution of diag rx
static uint16_t diag_rx_lastExec = 0u;

// main function
int main()
{
  // initialize everything
  boardInit();

  // main loop
  while(true) {
    uint16_t current_timer_val = getTimerVal();

    // check if controller should be executed
    // flag is set by ADC (which runs at 100Hz) after it has finished converting
    // TODO: fix execution of controller also via counters
    if (runController) {
      runController = false;
      run_system();
    }

    // check if diag rx should be executed
    // based on slow 1s timer which triggers tx uart
    if (current_timer_val - diag_rx_lastExec >= t_step_rx) {
      diag_rx_lastExec = current_timer_val;
      diag_step_100ms();
    }

    if (diag_tx_send) {
      diag_tx_send = false;
      diag_step_1000ms();
    }
  }
}
