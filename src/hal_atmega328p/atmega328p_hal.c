
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
  uint32_t sreg = SREG;
  cli();
  return sreg;
}

uint16_t getTimerVal() {
  // disable interrupts, fetch main coutner 16bit value and restore interrupts
  uint32_t sreg = disableInterrupts();
  uint16_t current_timer_val = (uint16_t) TIMER_VAL;
  RESTORE_SREG(sreg);
  return current_timer_val;
}

// encoder interrupt
ISR(INT0_vect) {
  incrementEncoder();
}

// reset watchdog timer
void resetWatchdog() {
  wdt_reset();
}
