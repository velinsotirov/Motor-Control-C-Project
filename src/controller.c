#include <stddef.h>
#include <math.h>
#include "global.h"
#include "controller.h"
#include "encoder.h"
#include "fixed_point.h"

// constants
static q8_8_t Kp = FLOAT_TO_Q8_8(0.033f);
static q8_8_t Ki = FLOAT_TO_Q8_8(0.5f);

// states
static q8_8_t integrator = 0;
static q8_8_t duty_cycle = 0;

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
    integrator = FLOAT_TO_Q8_8(0.0f);
}

void controller_update(q10_6_t speed_ref, q8_8_t uBat) {
    if (motorSpeed_get == NULL || dutyCycle_set == NULL) return;

    // get encoder speed estimate
    q10_6_t speed_meas = motorSpeed_get();

    // calculate control error
    q10_6_t speed_error = speed_ref - speed_meas;

    q8_8_t speed_error_q8_8 = Q10_6_TO_Q8_8(speed_error);

    // calculate PI control
    q8_8_t controller_P = q8_8_mul(speed_error_q8_8,Kp);
    integrator += q8_8_mul(speed_error_q8_8,Ts_controller);
    q8_8_t controller_I = q8_8_mul(integrator,Ki);
    q8_8_t controller = controller_P + controller_I;

    // limit duty cycle to [0,1]
    q8_8_t duty_unlim = q8_8_div(controller,uBat);
    q8_8_t duty_lim = fmaxf(duty_unlim,zero_q8_8);
    duty_lim = fminf(duty_lim,one_q8_8);

    // set duty cycle
    dutyCycle_set(duty_lim);
}

void set_motor_duty(q8_8_t duty) {
    duty_cycle = duty;
}

q8_8_t get_motor_duty(void) {
    return duty_cycle;
}

void reset_duty(void) {
    duty_cycle = zero_q8_8;
}