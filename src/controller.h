#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include "fixed_point.h"

extern int8_t duty;

extern const uint16_t t_step_controller;
extern int16_t controller_lastExec;

void speed_controller_step();
void torque_controller_step();

typedef int16_t (*GetMotorSpeed) (void);
typedef q4_12_t (*GetMotorCurrent) (void);
typedef void (*SetDuty) (int8_t);
typedef void (*ControllerRun) (void);

void attach_controller(GetMotorSpeed, GetMotorCurrent, SetDuty);
void detach_controller(void);
void set_motor_duty(int8_t);
int8_t get_motor_duty(void);
void reset_duty(void);
void resetSpeedIntegrator(void);
void resetCurrentIntegrator(void);

#endif // CONTROLLER_H