
#include <avr/io.h>
#include <avr/interrupt.h>

#include "init.h"
#include "uart.h"
#include "pinmap.h"

// setup everything
void boardInit() {
  // setup pins
  DDRD = 0b0;

  // PWM pins are outputs, encoderA and encoderB are inputs
  SET(DDRD, MASK(LB_HS) | MASK(LB_LS) | MASK(RB_HS) | MASK(RB_LS));

  // current measurement disabled for now
  //pinMode(shuntMeasR, INPUT); pinMode(shuntMeasL, INPUT);
  
  // HS: LOW is floating, HIGH is pulled to 12V
  // LS: LOW is floating, HIGH is pulled to GND
  // high impedance MCU pin state causes LS pulled to GND, HS floating
  
  // start with LS grounded to charge bootstrap caps
  RESET(DDRD, MASK(LB_HS) | MASK(RB_HS)); // HS floating
  SET(DDRD, MASK(LB_LS) | MASK(RB_LS)); // LW grounded

  // setup interrupts
  setPWMTimerInterrupt();
  setupTimeCounterInterrupt();
  //attachInterrupt(digitalPinToInterrupt(encoderA), incrementEncoder, RISING);

  // setup uart
  uart_init();
  
  // show program start, inactive for now
  //Serial.begin(9600);
  //Serial.println("\n---PROGRAM START---");
  
  // enable interrupts
  sei();
}

void setupTimeCounterInterrupt() {
  // interrupt for counting in 10Hz and 100Hz
  // disable interrupts
  cli();
 
  // reset timer count
  TCNT1 = 0;

  // clear timer on compare mode (WGM21 = 1)
  TCCR1A = 0b0;
  TCCR1A = (1 << WGM21); 

  // prescaler = 256 (CS12=1)
  TCCR1B = 0b0;
  TCCR1B = (1 << CS12);

  // 16MHz / (256 * (6249 + 1)) -> 10Hz
  OCR1A = 6249;

  // enable Timer1 Compare Match A interrupt
  TIMSK1 = 0b0;
  TIMSK1 |= (1 << OCIE1A);
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
