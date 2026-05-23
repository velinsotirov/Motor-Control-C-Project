#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include "global.h"
#include "controller.h"
#include "fixed_point.h"

// duty output from PI controller, with initial condition
int8_t duty = 0;

// constants
static const float Ki_float = 0.5f;
static const q22_10_t Kp = FLOAT_TO_Q22_10(0.01f);
static const q22_10_t Ki = FLOAT_TO_Q22_10(Ki_float);

// states
static q22_10_t integrator = 0;

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
    integrator = FLOAT_TO_Q22_10(0.0f);
}

void controller_update(int16_t speed_ref) {
    if (motorSpeed_get == NULL || dutyCycle_set == NULL) return;

    // get encoder speed estimate
    int16_t speed_meas = motorSpeed_get();

    // calculate control error
    int16_t speed_error = speed_ref - speed_meas;

    q22_10_t speed_error_q16_16 = INT_TO_Q22_10(speed_error);

    // calculate PI control
    q22_10_t controller_P = q22_10_mul(speed_error_q16_16,Kp);
    integrator += q22_10_mul(speed_error_q16_16,Ts_controller);
    q22_10_t controller_I = q22_10_mul(integrator,Ki);
    int16_t duty_unlim = Q22_10_TO_INT(controller_P + controller_I);

    // limit duty cycle
    if (duty_unlim > duty_max) {
        duty_unlim = duty_max;
    }
    else if (duty_unlim < duty_min) {
        duty_unlim = duty_min;
    }
    int8_t duty_lim = (int8_t) duty_unlim;

    // set duty cycle
    dutyCycle_set(duty_lim); //duty_lim
}

void set_motor_duty(int8_t duty_cycle) {
    duty = duty_cycle;
}

int8_t get_motor_duty(void) {
    return duty;
}

void reset_duty(void) {
    duty = 0u;
}