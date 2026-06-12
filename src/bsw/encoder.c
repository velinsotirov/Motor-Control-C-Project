#include <stdint.h>

#include "encoder.h"
#include "fixed_point.h"

#define GEAR_RATIO 35u
#define PPR 11u
#define ENCODER_CONV_FACT (GEAR_RATIO * PPR * 60u / 100u)
// if we wanted a higher accuracy, we could count encoder B pulses as well!

#ifdef __AVR_ATmega328P__
#include "atmega328p_hal.h"
// encoder count incremented by encoder interrupt
volatile int16_t encoder_cnt = 0;
#elif defined(__ARM_CortexM3__)
#include "stm32_hal.h"
#endif

// encoder last calculated speed
int16_t encoder_speed_rpm = 0;

// speed estimation, claled every 100ms in main loop
int16_t calculateSpeed() {
  int16_t encoderCnt = fetchAndResetEncoderCount();
  encoder_speed_rpm = encoderCnt / ENCODER_CONV_FACT;
  return encoder_speed_rpm;
}

int16_t get_motor_speed_est() {
  return encoder_speed_rpm;
}
