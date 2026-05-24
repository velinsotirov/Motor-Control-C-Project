
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pwm.h"
#include "init.h"
#include "diag.h"
#include "system.h"
#include "encoder.h"
#include "global.h"
#include "controller.h"
#include "pinmap.h"

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
    // read 16 bit timer
    cli(); uint16_t current_timer_val = TCNT1; sei();

    // check if controller should be executed
    if (current_timer_val - controller_lastExec >= t_step_controller) {
      controller_lastExec = current_timer_val;
      run_system();
    }

    // check if diag should be executed
    if (diag_Execute) {
      diag_Execute = false;
      diag_step();
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

// executes every 10ms and triggers diag_read
void mainCount() {
  diag_Execute = true;
}

// interrupt coutns for diag and controller execution
ISR(TIMER1_COMPA_vect) {
  mainCount(); 
}

// interrupt calls pwm counter
ISR(TIMER2_COMPA_vect) {
  pwmCount(); 
}
