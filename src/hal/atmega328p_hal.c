
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pwm.h"
#include "diag.h"
#include "encoder.h"
#include "atmega328p_hal.h"
#include "atmega328p_init.h"
#include "atmega328p_uart.h"

// disable interrupts and return their status
inline uint8_t disableInterrupts() {
  uint8_t sreg = SREG;
  cli();
  return sreg;
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
