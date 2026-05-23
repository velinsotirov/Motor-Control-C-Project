#include "pwm.h"
#include "global.h"
#include "controller.h"
#include "system.h"

#include <avr/io.h>

// set commands to all 4 MOSFETs based on duty cycle request
void pwmCommands() {

  // positive duty cycle
  if (duty >= 0) {
    // FET commands
    FET_cmd = 0b1000;
    duty_compa = duty;
    // toggle left side only
    //duty_L_H = 1;
    //duty_L_L = 0;
    // positive duty cycle, clamp bottom right to GND
    //duty_R_H = 0;
    //duty_R_L = 0;
  }
  // negative duty cycle
  else {
    // duty_compa is set to -duty since we count from 0, but FET_cmd is different
    // FET commands
    FET_cmd = 0b0010;
    duty_compa = -duty;
    // left side lower clamped to GND
    //duty_L_H = 0;
    //duty_L_L = 0;
    // negative duty cycle, clamp bottom left to GND
    //duty_R_H = 1;
    //duty_R_L = 0;
  }
}

// pwm counter
void pwmCount() {
  // check if powerstage is allowed
  if (get_controller_state() == STATE_RUNNING) {
    // increment the counter
    pwmCounter++;

    // turn HIGH at start of duty cycle
    if (pwmCounter >= duty_mean) { // >= duty_count
      pwmCounter = 0;
      // read shunt current
      //currentADCticks = analogRead(shuntMeas);

      // Start with HIGH (UDC -> LoadPos -> GND)
      if (duty_compa > 0) { // > duty 
        // previous FET_cmd = 0b0010
        
        // closed switches are opened so everything floats
        digitalWrite(LB_LS, !(FET_cmd & (1 << 3))); //LOW
        digitalWrite(RB_LS, !(FET_cmd & (1 << 1))); //HIGH
        // wait for MOSFETs to fully open
        delayMicroseconds(tdead_us);
        digitalWrite(LB_HS, FET_cmd & (1 << 3)); //HIGH
        digitalWrite(RB_HS, FET_cmd & (1 << 1)); //LOW
      }
    }
    // go to LOW if we reach the desired duty cycle value
    else if (pwmCounter == duty_compa) { // == duty
      // closed switches are opened so everything floats
      digitalWrite(LB_HS, FET_cmd & (1 << 2)); //LOW
      digitalWrite(RB_HS, FET_cmd & (1 << 0)); //HIGH
      // wait for MOSFETs to fully open
      delayMicroseconds(tdead_us);
      // close the others
      digitalWrite(LB_LS, !(FET_cmd & (1 << 2))); //HIGH
      digitalWrite(RB_LS, !(FET_cmd & (1 << 0))); //LOW
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