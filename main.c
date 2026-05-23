#include <stdbool.h>

#include "pwm.h"
#include "system.h"
#include "encoder.h"

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
const float ampGain = 18.0;
const float shuntR = 0.5;
const float voltageGain = float(5)/float(1023);

// user inputs
int16_t desiredSpeed_rpm = 100;

// measurements
int16_t measuredSpeed_rpm = 0;
volatile uint16_t currentADCticks = 0;
float measuredCurrent = 0.0;

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
      run_system(desiredSpeed_rpm);

      // measure current
      if (duty >= 0)
        currentADCticks = analogRead(shuntMeasR);
      else
        currentADCticks = -analogRead(shuntMeasL);
      measuredCurrent = (float) (currentADCticks*voltageGain/(ampGain*shuntR));
      
      // print runtime, speed, duty and current
      Serial.print("\nSpeed: ");
      Serial.print(measuredSpeed_rpm);
      Serial.print("\nDuty: ");
      Serial.print(duty);
      Serial.print("\nCurrent: ");
      Serial.print(measuredCurrent);
      t_lastcycle = t_runtime;
    }
  }
}

// setup everything
void setup() {
  // setup pins
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  pinMode(shuntMeasR, INPUT);
  pinMode(shuntMeasL, INPUT);
  pinMode(LB_HS, OUTPUT);
  pinMode(LB_LS, OUTPUT); 
  pinMode(RB_HS, OUTPUT);
  pinMode(RB_LS, OUTPUT);
  
  // HS: LOW is floating, HIGH is pulled to 12V
  // LS: LOW is floating, HIGH is pulled to GND
  // high impedance MCU pin state causes LS pulled to GND, HS floating
  
  // start with LS grounded to charge bootstrap caps
  digitalWrite(LB_HS, LOW);
  digitalWrite(RB_HS, LOW);
  digitalWrite(LB_LS, HIGH); 
  digitalWrite(RB_LS, HIGH);

  // PWM interrupt
  setPWMTimerInterrupt();
  
  // encoder interrupt
  attachInterrupt(digitalPinToInterrupt(encoderA), incrementEncoder, RISING);
  
  // show program start
  Serial.begin(9600);
  Serial.println("\n---PROGRAM START---");
  
  // enable interrupts (PWM timer)
  sei();
}

void setupTimeCounterInterrupt() {
  // interrupt should count every 100ms

}