#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include "fixed_point.h"

extern const uint8_t init_ticks;

extern const q22_10_t Ts_controller;
extern const int16_t max_speed;

extern const uint8_t max_current;

extern const q22_10_t one_q16_16;
extern const q22_10_t zero_q16_16;

// duty parameters
extern const int8_t duty_count;
extern const int8_t duty_mean;
extern const int8_t duty_min;
extern const int8_t duty_max;

#endif // GLOBAL_H