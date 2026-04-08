#include <stddef.h>
#include "global.h"
#include "controller.h"
#include "encoder.h"

// constants
static const float Kp = 0.033f;
static const float Ki = 0.5f;


// states
static float integrator = 0.0f;
static float duty_cycle = 0.0f;

static GetMotorSpeed motorSpeed_get = NULL;
static SetDuty dutyCycle_set = NULL;

void attach_controller(GetMotorSpeed s, SetDuty d) {
    motorSpeed_get = s;
    dutyCycle_set = d;
}

void detach_controller(void) {
    motorSpeed_get = NULL;
    dutyCycle_set = NULL;
    reset_duty();
    integrator = 0.0f;
}

void controller_update(float speed_ref, float uBat) {
    if (motorSpeed_get == NULL || dutyCycle_set == NULL) return;

    // get encoder speed estimate
    float speed_meas = motorSpeed_get();

    // calculate control error
    float speed_error = speed_ref - speed_meas;

    // calculate PI control
    float controller_P = speed_error * Kp;
    integrator += speed_error * Ts_controller;
    float controller_I = integrator * Ki;
    float controller = controller_P + controller_I;

    // limit duty cycle to [0,1]
    float duty_unlim = controller/uBat;
    float duty_lim = fmaxf(duty_unlim,0.0f);
    duty_lim = fminf(duty_lim,1.0f);

    // set duty cycle
    dutyCycle_set(duty_lim);
}

void set_motor_duty(float duty) {
    duty_cycle = duty;
}

float get_motor_duty(void) {
    return duty_cycle;
}

void reset_duty(void) {
    duty_cycle = 0.0f;
}