#include "pwm.h"
#include "global.h"
#include "controller.h"
#include "system.h"

#include <avr/io.h>

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
      // read shunt current
      //currentADCticks = analogRead(shuntMeas);

      // if duty > 0, we start duty period with HIGH
      if (duty_compa > 0) {

        // if duty cycle is positive, LB toggles and RB is clamped to GND
        if (duty >= 0) {
          // closed are opened
          RESET(DDRD, MASK(RB_HS)); // RB high is open
          RESET(DDRD, MASK(LB_LS)); // open LB low
          // wait for dead time
          DELAY_0_4US();
          // opened are closed
          SET(DDRD, MASK(RB_LS)); // RB low is closed
          SET(DDRD, MASK(LB_HS)); // close LB high
        }
        // if duty cycle is negative, LB is clamped to GND and RB toggles
        else {
          // closed are opened
          RESET(DDRD, MASK(LB_HS)); // LB high is open
          RESET(DDRD, MASK(RB_LS)); // open RB low
          // wait for dead time
          DELAY_0_4US();
          // opened are closed
          SET(DDRD, MASK(LB_LS)); // LB low is closed
          SET(DDRD, MASK(RB_HS)); // close RB high
        }
      }
    }
    // go to LOW if we reach the desired duty cycle value
    else if (pwmCounter == duty_compa) {
      // if duty cycle is negative, LB toggles and RB is clamped to GND
      if (duty >= 0) {
        // closed are opened
          RESET(DDRD, MASK(RB_HS)); // RB high is open
          RESET(DDRD, MASK(LB_HS)); // open LB high
          // wait for dead time
          DELAY_0_4US();
          // opened are closed
          SET(DDRD, MASK(RB_LS)); // RB low is closed
          SET(DDRD, MASK(LB_LS)); // close LB low
      }
      // if duty cycle is negative, LB is clamped to GND and RB toggles
      else {
        // closed are opened
        RESET(DDRD, MASK(LB_HS)); // LB high is open
        RESET(DDRD, MASK(RB_HS)); // open RB high
        // wait for dead time
        DELAY_0_4US();
        // opened are closed
        SET(DDRD, MASK(LB_LS)); // LB low is closed
        SET(DDRD, MASK(RB_LS)); // close RB low
      }
    }
  }
}

// interrupt calls pwm counter
ISR(TIMER2_COMPA_vect) {
  pwmCount(); 
}

// set up interrupt which triggers PWM timer
void setPWMTimerInterrupt() {
  // disable interrupts
  cli();
 
  // reset timer count
  TCNT2 = 0;

  // clear timer on compare mode (WGM21 = 1)
  TCCR2A = 0b0;
  TCCR2A = (1 << WGM21); 

  // prescaler = 1 (CS20=1)
  TCCR2B = 0b0;
  TCCR2B = (1 << CS20);

  // 16MHz / (1 * (31 + 1)) = 500kHz -> 20kHz PWM when we count to 25
  OCR2A = 31;

  // enable Timer2 Compare Match A interrupt
  TIMSK2 = 0b0;
  TIMSK2 |= (1 << OCIE2A);
}