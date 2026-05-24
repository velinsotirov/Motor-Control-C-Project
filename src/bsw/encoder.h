#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#include "fixed_point.h"

int16_t calculateSpeed(void);

void incrementEncoder(void);

int16_t get_motor_speed_est(void);

#endif // ENCODER_H