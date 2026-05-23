#include <stdint.h>

#include "fixed_point.h"
#include "math.h"
#include "global.h"

const uint8_t init_ticks = 1u;

const q16_16_t Ts_controller = FLOAT_TO_Q16_16(0.01f); // 10ms control time step
const int16_t max_speed = 10000u;

const q16_16_t one_q16_16 = FLOAT_TO_Q16_16(1.0f);
const q16_16_t zero_q16_16 = FLOAT_TO_Q16_16(0.0f);

// duty parameters
const int8_t duty_count = 200u;
const int8_t duty_mean = 100u;
const int8_t duty_min = -96;
const int8_t duty_max = 96;

// dead time
const uint16_t tdead_us = 2;
