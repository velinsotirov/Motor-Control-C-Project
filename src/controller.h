#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "fixed_point.h"

extern const uint16_t t_step_controller;
extern int16_t duty;

extern volatile bool runController;

void speed_controller_step();
void torque_controller_step();

typedef int16_t (*GetMotorSpeed) (void);
typedef q4_12_t (*GetMotorCurrent) (void);
typedef void (*SetDuty) (int16_t);
typedef void (*ControllerRun) (void);

void attach_controller(GetMotorSpeed, GetMotorCurrent, SetDuty);
void detach_controller(void);
int16_t get_motor_duty(void);
void reset_duty(void);
void resetSpeedIntegrator(void);
void resetCurrentIntegrator(void);

#endif // CONTROLLER_H