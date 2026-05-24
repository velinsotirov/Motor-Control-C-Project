
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pwm.h"
#include "init.h"
#include "system.h"
#include "encoder.h"
#include "global.h"
#include "controller.h"
#include "pinmap.h"

// allow powerstage after certain time
int16_t t_runtime = 0;
int16_t t_lastcycle = 0;

// time step of controller
const int16_t t_step_controller = 624;

// current measurement constants
const float ampGain = 18.0f;
const float shuntR = 0.5f;
const float voltageGain = (float) 5 / (float) 1023;

// user inputs
int16_t desiredSpeed_rpm = 1000;
q4_12_t desiredTorque = FLOAT_TO_Q4_12(0.01f); // approx. half of max torque

// measurements
int16_t measuredSpeed_rpm = 0;
volatile uint16_t currentADCticks = 0;
float measuredCurrent = 0.0;

// controller and diag previous execution counters
int16_t controller_lastExec = 0;
volatile bool diag_Execute = false;

// main function
int main()
{
  // initialize everything
  boardInit();

  // main loop
  while(true) {
    // check if controller should be executed
    if (TCNT1 - controller_lastExec >= t_step_controller) {
      controller_lastExec = TCNT1;
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

// executes every 10ms
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
