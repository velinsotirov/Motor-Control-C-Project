#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "fixed_point.h"

extern int8_t duty;

void controller_update(int16_t);

typedef int16_t (*GetMotorSpeed) (void);

typedef void (*SetDuty) (int8_t);

void attach_controller(GetMotorSpeed, SetDuty);

void detach_controller(void);

void set_motor_duty(int8_t);

int8_t get_motor_duty(void);

void reset_duty(void);

#endif // CONTROLLER_H