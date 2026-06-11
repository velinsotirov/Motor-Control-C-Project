#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include "fixed_point.h"

extern const q22_10_t Ts_controller;
extern const int16_t max_speed;

extern const uint8_t max_current;

extern const float K_times_Psi;
extern const q4_12_t K_times_Psi_q4_12;

// duty parameters
extern const int8_t duty_mean;
extern const int8_t duty_min;
extern const int8_t duty_max;

#endif // GLOBAL_H