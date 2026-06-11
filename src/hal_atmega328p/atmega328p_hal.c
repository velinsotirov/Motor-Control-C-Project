
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "diag.h"
#include "encoder.h"
#include "atmega328p_hal.h"
#include "atmega328p_pwm.h"
#include "atmega328p_init.h"
#include "atmega328p_uart.h"

// disable interrupts and return their status
inline uint32_t disableInterrupts() {
  uint32_t sreg = (uint32_t) SREG;
  cli();
  return sreg;
}

uint32_t getTimerVal() {
  // disable interrupts, fetch main coutner 16bit value and restore interrupts
  uint32_t sreg = disableInterrupts();
  uint32_t current_timer_val = (uint32_t) TIMER_VAL;
  RESTORE_SREG(sreg);
  return current_timer_val;
}

// encoder interrupt
// encoder counts every time encoder A rises
// count up if spinning forwards (B is positive), else count down
ISR(INT0_vect) {
  if (HAL_IS_PIN_HIGH(PIN_ENCODER_B)) {
    encoder_cnt += 1;
  }
  else {
    encoder_cnt -= 1;
  }
}

// return encoder count and reset it
int16_t fetchAndResetEncoderCount() {
  int16_t temp_cnt = encoder_cnt;
  encoder_cnt = 0;
  return temp_cnt;
}

// reset watchdog timer
void resetWatchdog() {
  wdt_reset();
}
