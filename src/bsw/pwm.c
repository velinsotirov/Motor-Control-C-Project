#include "pwm.h"
#include "global.h"
#include "controller.h"
#include "system.h"
#include "atmega328p_hal.h"

// duty which is read by PWM interrupt
volatile uint8_t duty_compa = 0;

// pwm counter
volatile uint8_t pwmCounter = 0;

// pwm counter
void pwmCount() {
  // check if powerstage is allowed
  if (get_controller_state() == STATE_RUNNING) {
    // increment the counter
    pwmCounter++;

    // compare value is positive
    if (duty >= 0) {
      duty_compa = duty;
    }
    else {
      duty_compa = -duty;
    }

    // if we reach the end of the PWM period, reset counter
    if (pwmCounter >= duty_mean) { // >= duty_count
      pwmCounter = 0;

      // if duty > 0, we start duty period with HIGH
      if (duty_compa > 0) {

        // if duty cycle is positive, LB toggles and RB is clamped to GND
        if (duty >= 0) {
          // closed are opened
          HAL_RESET_PIN(PIN_RIGHT_HS); // RB high is open
          HAL_RESET_PIN(PIN_LEFT_LS); // open LB low
          // wait for dead time
          DELAY_0_4US();
          // opened are closed
          HAL_SET_PIN(PIN_RIGHT_LS); // RB low is closed
          HAL_SET_PIN(PIN_LEFT_HS); // close LB high
        }
        // if duty cycle is negative, LB is clamped to GND and RB toggles
        else {
          // closed are opened
          HAL_RESET_PIN(PIN_LEFT_HS); // LB high is open
          HAL_RESET_PIN(PIN_RIGHT_LS); // open RB low
          // wait for dead time
          DELAY_0_4US();
          // opened are closed
          HAL_SET_PIN(PIN_LEFT_LS); // LB low is closed
          HAL_SET_PIN(PIN_RIGHT_HS); // close RB high
        }
      }
    }
    // go to LOW if we reach the desired duty cycle value
    else if (pwmCounter == duty_compa) {
      // if duty cycle is negative, LB toggles and RB is clamped to GND
      if (duty >= 0) {
        // closed are opened
          HAL_RESET_PIN(PIN_RIGHT_HS); // RB high is open
          HAL_RESET_PIN(PIN_LEFT_HS); // open LB high
          // wait for dead time
          DELAY_0_4US();
          // opened are closed
          HAL_SET_PIN(PIN_RIGHT_LS); // RB low is closed
          HAL_SET_PIN(PIN_LEFT_LS); // close LB low
      }
      // if duty cycle is negative, LB is clamped to GND and RB toggles
      else {
        // closed are opened
        HAL_RESET_PIN(PIN_LEFT_HS); // LB high is open
        HAL_RESET_PIN(PIN_RIGHT_HS); // open RB high
        // wait for dead time
        DELAY_0_4US();
        // opened are closed
        HAL_SET_PIN(PIN_LEFT_LS); // LB low is closed
        HAL_SET_PIN(PIN_RIGHT_LS); // close RB low
      }
    }
  }
}
