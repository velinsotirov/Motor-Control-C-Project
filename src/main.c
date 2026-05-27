
#include <stdint.h>
#include <stdbool.h>

#include "pwm.h"
#include "diag.h"
#include "system.h"
#include "global.h"
#include "encoder.h"
#include "controller.h"
#include "atmega328p_init.h"
#include "atmega328p_hal.h"

// current measurement constants
const float ampGain = 18.0f;
const float shuntR = 0.5f;
const float voltageGain = (float) 5 / (float) 1023;

// measurements
int16_t measuredSpeed_rpm = 0;
volatile uint16_t currentADCticks = 0;
float measuredCurrent = 0.0;

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
    if (current_timer_val - controller_lastExec >= t_step_controller) {
      controller_lastExec = current_timer_val;
      run_system();
    }

    // check if diag rx should be executed
    if (current_timer_val - controller_lastExec >= t_step_rx) {
      diag_step_100ms();
    }

    if (diag_tx_send) {
      diag_tx_send = false;
      diag_step_1000ms();
    }
    
    // measure current
    /*
    if (duty >= 0)
      currentADCticks = analogRead(shuntMeasR);
    else
      currentADCticks = -analogRead(shuntMeasL);
    measuredCurrent = (float) (currentADCticks*voltageGain/(ampGain*shuntR));
      */
  }
}
