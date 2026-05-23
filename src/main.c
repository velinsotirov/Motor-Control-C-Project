
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pwm.h"
#include "system.h"
#include "encoder.h"
#include "global.h"
#include "controller.h"

#define LB_HS 4
#define LB_LS 5
#define RB_HS 6
#define RB_LS 7
#define encoderA 2
#define encoderB 8
#define shuntMeasR A0
#define shuntMeasL A1

// allow powerstage after certain time
int16_t t_runtime = 0;
int16_t t_lastcycle = 0;

// time step of main code
const int16_t t_step = 99;

// current measurement constants
const float ampGain = 18.0f;
const float shuntR = 0.5f;
const float voltageGain = (float) 5 / (float) 1023;

// user inputs
int16_t desiredSpeed_rpm = 1000;
q4_12_t desiredTorque = FLOAT_TO_Q4_12(0.01f); // approx. half of max torque

// measurements
int16_t measuredSpeed_rpm = 0;
volatile uint16_t currentADCticks = 0;
float measuredCurrent = 0.0;

// setup function exists
void setup(void);

// main function
int main()
{
  setup();

  // main loop
  while(true) {
    // current runtime
    t_runtime = millis();
    
    // set duty and print to console every t_step
    if (t_runtime - t_lastcycle > t_step) {
      
      // step system logic
      run_system(false,desiredSpeed_rpm);

      // measure current
      /*
      if (duty >= 0)
        currentADCticks = analogRead(shuntMeasR);
      else
        currentADCticks = -analogRead(shuntMeasL);
      measuredCurrent = (float) (currentADCticks*voltageGain/(ampGain*shuntR));
      */
      
      // print runtime (inactive for now), speed, duty and current
      /*
      Serial.print("\nSpeed: ");
      Serial.print(measuredSpeed_rpm);
      Serial.print("\nDuty: ");
      Serial.print(duty);
      Serial.print("\nCurrent: ");
      Serial.print(measuredCurrent);
      */
      t_lastcycle = t_runtime;
    }
  }
}

// setup everything
void setup() {
  // setup pins
  DDRD = 0b0;

  // PWM pins are outputs, encoderA and encoderB are inputs
  SET(DDRD, MASK(LB_HS) | MASK(LB_LS) | MASK(RB_HS) | MASK(RB_LS));

  // current measurement disabled for now
  //pinMode(shuntMeasR, INPUT);
  //pinMode(shuntMeasL, INPUT);
  
  // HS: LOW is floating, HIGH is pulled to 12V
  // LS: LOW is floating, HIGH is pulled to GND
  // high impedance MCU pin state causes LS pulled to GND, HS floating
  
  // start with LS grounded to charge bootstrap caps
  RESET(DDRD, MASK(LB_HS) | MASK(RB_HS)); // HS floating
  SET(DDRD, MASK(LB_LS) | MASK(RB_LS)); // LW grounded

  // PWM interrupt
  setPWMTimerInterrupt();
  
  // encoder interrupt (must be done via registers!)
  //attachInterrupt(digitalPinToInterrupt(encoderA), incrementEncoder, RISING);
  
  // show program start, inactive for now
  //Serial.begin(9600);
  //Serial.println("\n---PROGRAM START---");
  
  // enable interrupts (PWM timer)
  sei();
}

void setupTimeCounterInterrupt() {
  // interrupt should count every 10ms so we can run controller at 100Hz

}