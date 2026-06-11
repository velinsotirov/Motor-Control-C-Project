
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
// system clock
#define F_CPU 16000000UL
#else
#include "stm32_init.h"
#include "stm32_hal.h"
#endif

// last execution of controller, diag rx and tx
static uint32_t controller_lastExec = 0u;
static uint32_t diag_rx_lastExec = 0u;
static uint32_t diag_tx_lastExec = 0u;

// main function
int main()
{
  // initialize everything
  boardInit();

  // start watchdog
  setupWatchdog();

  // main loop
  while(1) {
    uint32_t current_timer_val = getTimerVal();

    // check if controller should be executed
    if (current_timer_val - controller_lastExec >= t_step_controller) {
      controller_lastExec = current_timer_val;
      run_system();

      // reset watchdog during the fastest task
      resetWatchdog();
    }

    // check if diag rx should be executed
    if (current_timer_val - diag_rx_lastExec >= t_step_rx) {
      diag_rx_lastExec = current_timer_val;
      diag_step_100ms();
    }

    if (current_timer_val - diag_tx_lastExec >= t_step_tx) {
      diag_tx_lastExec = current_timer_val;
      diag_step_1000ms();
    }
  }
}
