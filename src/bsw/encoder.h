#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#include "fixed_point.h"

int16_t calculateSpeed(void);
int16_t get_motor_speed_est(void);

extern volatile int16_t encoder_cnt;

#endif // ENCODER_H