
#include <avr/io.h>
#include <avr/interrupt.h>

#include "encoder.h"
#include "atmega328p_hal.h"
#include "atmega328p_uart.h"
#include "atmega328p_init.h"
#include "atmega328p_adc.h"

// setup encoder interrupt
void setupEncoderInterrupt() {
  // enable interrupt INT0 (pin 2)
  SET(EIMSK, MASK(INT0));

  // interrupt triggers on rising edge
  SET(EICRA, MASK(ISC01) | MASK(ISC00));
}

void setupTimeCounter() {
  // interrupt for counting at 500Hz for controller and diag rx/tx timing
 
  // reset timer count
  TCNT0 = 0;

  // normal mode
  TCCR0A = 0b0;

  // prescaler = 256 (CS02=1)
  TCCR0B = 0b0;
  TCCR0B = (1 << CS02);
}

// setup everything
void boardInit() {
  // disable interrupts
  cli();

  // HS: LOW is floating, HIGH is pulled to 12V
  // LS: LOW is floating, HIGH is pulled to GND
  // high impedance MCU pin state causes LS pulled to GND, HS floating

  // pwm pins do not use pullup resistors, since we have HW pullup/down resistors as needed!
  // however, LS starts closed in order to charge the bootstrap
  HAL_RESET_PIN(PIN_LEFT_HS);
  HAL_RESET_PIN(PIN_RIGHT_HS);
  HAL_SET_PIN(PIN_LEFT_LS);
  HAL_SET_PIN(PIN_RIGHT_LS);
  // setup PWM pins as outputs, executed after values are set for safety reasons
  SET_PIN_AS_OUTPUT(PIN_LEFT_HS);
  SET_PIN_AS_OUTPUT(PIN_LEFT_LS);
  SET_PIN_AS_OUTPUT(PIN_RIGHT_HS);
  SET_PIN_AS_OUTPUT(PIN_RIGHT_LS);
  
  // setup encoder pins as inputs by default, no DDRB/DDRC/DDRD reset needed
  // encoder pins without pull ups (we have HW pull down resistors)
  HAL_RESET_PIN(PIN_ENCODER_A);
  HAL_RESET_PIN(PIN_ENCODER_B);

  // current measurement as inputs by default, no DDRB/DDRC/DDRD reset needed
  // current measurement without pull up (its pulled down by op amp circuit)
  HAL_RESET_PIN(PIN_SHUNT_L);
  HAL_RESET_PIN(PIN_SHUNT_R);

  // UART pins will be overriden anyways by UART config, so no need to do anything

  // all unused pins as inputs, no need to reset DDRB/DDRC/DDRD as theyre inputs by default
  // all unused pins pulled high to avoid floating states
  SET(PORTB, (~USED_PINS_PORTB) & VALID_PINS_B);
  SET(PORTC, (~USED_PINS_PORTC) & VALID_PINS_C);
  SET(PORTD, (~USED_PINS_PORTD) & VALID_PINS_D);

  // setup interrupts
  setPWMTimer();
  setupTimeCounter();
  setupEncoderInterrupt();

  // setup uart and adc
  setupADC();
  uart_init();
  
  // enable interrupts
  sei();
}
