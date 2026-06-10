
#include <avr/io.h>
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

// interrupt for calling the ADC
ISR(TIMER0_COMPA_vect) {
  //diagTrigger();
}

// interrupt for calling tx uart, which also coutns for rx uart and controller execution
ISR(TIMER1_COMPA_vect) {
  diagTrigger(); 
}

// interrupt calls pwm counter
ISR(TIMER2_COMPA_vect) {
  pwmCount(); 
}

// encoder interrupt
ISR(INT0_vect)
{
  incrementEncoder();
}
