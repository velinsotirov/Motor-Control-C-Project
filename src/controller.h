#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "fixed_point.h"

void controller_update(q10_6_t, q8_8_t);

typedef q10_6_t (*GetMotorSpeed) (void);

typedef void (*SetDuty) (q8_8_t);

void attach_controller(GetMotorSpeed, SetDuty);

void detach_controller(void);

void set_motor_duty(q8_8_t);

q8_8_t get_motor_duty(void);

void reset_duty(void);

#endif // CONTROLLER_H