
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
#elif defined(__ARM_CortexM3__)
#include "stm32_hal.h"
#include "stm32_gpio.h"
#include "stm32_init.h"
#endif

// last execution of controller, diag rx and tx
// 32 bit needed even though avr timer is 16bit, because stm systick is 32bit
static uint32_t controller_lastExec = 0u;
static uint32_t diag_rx_lastExec = 0u;
static uint32_t diag_tx_lastExec = 0u;

static uint32_t system_lastExecTicks = 0u;
static uint32_t rxDiag_lastExecTicks = 0u;
static uint32_t txDiag_lastExecTicks = 0u;

// main function
int main()
{
  // initialize everything
  boardInit();

  // start watchdog
  //setupWatchdog();

  // enable interrupts
  enableInterrupts();

  // main loop
  while(1) {
    // 32 bit needed even though avr timer is 16bit, because stm systick is 32bit
    uint32_t current_timer_val = getTimerVal();

    /**/
    // check if controller should be executed
    if (current_timer_val - controller_lastExec >= t_step_controller) {
      controller_lastExec = current_timer_val;
      uint32_t start = get_cycles(); // for profiling
      run_system();
      uint32_t time =  get_cycles() - start;
      if (time > system_lastExecTicks) { system_lastExecTicks = time; } // for profiling

      // reset watchdog during the fastest task
      //resetWatchdog();
    }

    // check if diag rx should be executed
    if (current_timer_val - diag_rx_lastExec >= t_step_rx) {
      diag_rx_lastExec = current_timer_val;
      uint32_t start = get_cycles(); // for profiling
      diag_step_100ms();
      uint32_t time =  get_cycles() - start;
      if (time > rxDiag_lastExecTicks) { rxDiag_lastExecTicks = time; } // for profiling
    }
    

    if (current_timer_val - diag_tx_lastExec >= t_step_tx) {
      diag_tx_lastExec = current_timer_val;
      uint32_t start = get_cycles(); // for profiling
      diag_step_500ms();
      toggleLED();
      uint32_t time =  get_cycles() - start;
      if (time > txDiag_lastExecTicks) { txDiag_lastExecTicks = time; } // for profiling
    }
  }
}
