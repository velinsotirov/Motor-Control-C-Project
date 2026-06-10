
#include "global.h"
#include "system.h"
#include "controller.h"
#include "atmega328p_hal.h"
#include "atmega328p_pwm.h"

volatile bool dutyPositive = true;
volatile bool countingUp = false;

void set_duty_cycle(int16_t duty) {
  // is duty positive
  uint16_t duty_compa;
  if (duty >= 0) {
    dutyPositive = true;
    duty_compa = duty;
  }
  else {
    dutyPositive = false;
    duty_compa = -duty;
  }
  // we count to 400, so a duty of 100 is 400
  OCR1B = duty_compa * 4;
}

void disable_pwm() {
  TCNT1 = 0;
  RESET(TIMSK1, MASK(OCIE1A));
}

void enable_pwm() {
  SET(TIMSK1, MASK(OCIE1A));
}

ISR(TIMER1_COMPA_vect) {
  if (countingUp) {
    // we are upcounting, so we are transitioning from high to low on compare match
    // if duty cycle is negative, LB toggles and RB is clamped to GND
    if (dutyPositive) {
      // closed are opened
        HAL_RESET_PIN(PIN_RIGHT_HS); // RB high is open
        HAL_RESET_PIN(PIN_LEFT_HS); // open LB high
        // wait for dead time
        DELAY_0_5US();
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
      DELAY_0_5US();
      // opened are closed
      HAL_SET_PIN(PIN_LEFT_LS); // LB low is closed
      HAL_SET_PIN(PIN_RIGHT_LS); // close RB low
    }
  }
  else {
    // we are downcounting, so we are transitioning from low to high on compare match
    // if duty cycle is positive, LB toggles and RB is clamped to GND
    if (dutyPositive) {
      // closed are opened
      HAL_RESET_PIN(PIN_RIGHT_HS); // RB high is open
      HAL_RESET_PIN(PIN_LEFT_LS); // open LB low
      // wait for dead time
      DELAY_0_5US();
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
      DELAY_0_5US();
      // opened are closed
      HAL_SET_PIN(PIN_LEFT_LS); // LB low is closed
      HAL_SET_PIN(PIN_RIGHT_HS); // close RB high
    }
  }
  countingUp = false;

  // disable ADC interrupt so we dont trigger the ADC during the downcounting part
  RESET(TIMSK1, MASK(OCIE1B));
}

// set up interrupt which triggers PWM timer
void setupPWMTimer() {
  // reset timer count
  TCNT1 = 0;

  // Phase correct PWM, TOP = ICR1
  TCCR1A = 0b0;
  TCCR1B = 0b0;
  TCCR1A = (1 << WGM11);
  TCCR1B = (1 << WGM13);

  // prescaler = 1 (CS10=1)
  TCCR1B = (1 << CS10);

  // pwm frequency is 20khz, we have an up down counter with prescaler 1
  ICR1 = 400;

  // ADC starts sampling around middle of PWM period, and with a prescaler of 32, takes 1.5 cycles to sample,
  // which is 16MHz/32 = 500kHz, so 1.5 periods is 3us. this limits max duty to 94%
  OCR1B = 376;

  // enable Timer1 Compare Match A interrupt, this will be our manual PWM implemetation with SW dead time
  TIMSK1 = 0b0;
  SET(TIMSK1, MASK(OCIE1A));

  // this tells us when we start up counting
  SET(TIMSK1, MASK(TOIE1));

  // enable Timer1 Compare Match B interrupt, this will be our ADC sampling
  SET(TIMSK1, MASK(OCIE1B));
}

// ISR fires at start of PWM period, and lets us know we are counting up
ISR(TIMER1_OVF_vect) {
  countingUp = true;

  // enable ADC interrupt
  SET(TIMSK1, MASK(OCIE1B));
}
