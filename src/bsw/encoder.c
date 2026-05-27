#include <stdint.h>

#include "encoder.h"
#include "fixed_point.h"
#include "atmega328p_hal.h"

// tstep 0.1s and 12 PPR
const uint8_t speed_conv_fact = 50u;

// encoder last calculated speed
int16_t encoder_speed_rpm = 0;

// encoder count incremented by encoder interrupt
volatile int16_t encoder_cnt = 0;

// speed estimation, claled every 100ms in main loop
int16_t calculateSpeed() {
  encoder_speed_rpm = encoder_cnt * speed_conv_fact;
  encoder_cnt = 0;
  return encoder_speed_rpm;
}

// encoder counts every time encoder A rises
// count up if spinning forwards (B is positive), else count down
void incrementEncoder() {
  if (HAL_IS_PIN_HIGH(PIN_ENCODER_B)) {
    encoder_cnt += 1;
  }
  else {
    encoder_cnt -= 1;
  }
}

int16_t get_motor_speed_est() {
  return encoder_speed_rpm;
}
