
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

void setupTimeCounterInterrupt() {
  // interrupt for counting to 1s for DIAG TX
 
  // reset timer count
  TCNT1 = 0;

  // clear timer on compare mode (WGM21 = 1)
  TCCR1A = 0b0;
  TCCR1A = (1 << WGM21); 

  // prescaler = 256 (CS12=1)
  TCCR1B = 0b0;
  TCCR1B = (1 << CS12);

  // 16MHz / (256 * (62499 + 1)) -> 1Hz
  OCR1A = 62499;

  // enable Timer1 Compare Match A interrupt
  TIMSK1 = 0b0;
  TIMSK1 |= (1 << OCIE1A);
}

// set up interrupt which triggers PWM timer
void setPWMTimerInterrupt() {
  // reset timer count
  TCNT2 = 0;

  // clear timer on compare mode (WGM21 = 1)
  TCCR2A = 0b0;
  TCCR2A = (1 << WGM21); 

  // prescaler = 1 (CS20=1)
  TCCR2B = 0b0;
  TCCR2B = (1 << CS20);

  // 16MHz / (1 * (7 + 1)) = 2MHz -> 20kHz PWM when we count to 100
  OCR2A = 7;

  // enable Timer2 Compare Match A interrupt
  TIMSK2 = 0b0;
  TIMSK2 |= (1 << OCIE2A);
}

void setupADCCounterInterrupt() {
  // interrupt which triggers every 10ms and triggers ADC conversion
 
  // reset timer count
  TCNT0 = 0;

  // clear timer on compare mode
  TCCR0A = 0b0;
  TCCR0A = (1 << WGM01);

  // prescaler 1024
  TCCR0B = 0b0;
  TCCR0B = (1 << CS02) | (1 << CS00);

  // 16MHz / (1024 * (155 + 1)) -> 100Hz approx
  OCR0A = 155;

  // enable Timer0 Compare Match A interrupt
  TIMSK0 = 0b0;
  TIMSK0 |= (1 << OCIE0A);
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
  setPWMTimerInterrupt();
  setupTimeCounterInterrupt();
  setupEncoderInterrupt();
  setupADCCounterInterrupt();
  //setupADCInterrupt();

  // setup uart and adc
  setupADC();
  uart_init();
  
  // enable interrupts
  sei();
}
