
#include <stdint.h>
#include <stdbool.h>

#include "pwm.h"
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
    // disable interrupts, fetch main coutner 16bit value and restore interrupts
    uint8_t sreg = disableInterrupts();
    uint16_t current_timer_val = TIMER_VAL;
    RESTORE_SREG(sreg);

    // check if controller should be executed
    // flag is set by ADC (which runs at 100Hz) after it has finished converting
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
