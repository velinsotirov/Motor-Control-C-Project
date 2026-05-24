#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include "global.h"
#include "system.h"
#include "motor.h"
#include "controller.h"
#include "fixed_point.h"

// duty output from controller, with initial condition
int8_t duty = 0;

// speed controller constants
static const float Ki_float = 0.5f;
static const q22_10_t Kp = FLOAT_TO_Q22_10(0.01f);
static const q22_10_t Ki = FLOAT_TO_Q22_10(Ki_float);

// torque controller constants
static const float Kit_float = 2500.0f;
static const q22_10_t Kpt = FLOAT_TO_Q22_10(10.0f);
static const q22_10_t Kit = FLOAT_TO_Q22_10(Kit_float);

// speed controller states, static for file so they can be reset when detaching
static q22_10_t integrator_speed = 0;
static q22_10_t integrator_current = 0;

// function pointers
static GetMotorSpeed motorSpeed_get = NULL;
static GetMotorCurrent motorCurrent_get = NULL;
static SetDuty dutyCycle_set = NULL;
static ControllerRun runController = NULL;

void attach_controller(GetMotorSpeed s, GetMotorCurrent c, SetDuty d) {
    motorSpeed_get = s;
    motorCurrent_get = c;
    dutyCycle_set = d;
}

void detach_controller(void) {
    motorSpeed_get = NULL;
    motorCurrent_get = NULL;
    dutyCycle_set = NULL;
    reset_duty();
    integrator_speed = FLOAT_TO_Q22_10(0.0f);
    integrator_current = FLOAT_TO_Q22_10(0.0f);
}

void speed_controller_step() {
    if (motorSpeed_get == NULL || dutyCycle_set == NULL) return;

    // get encoder speed estimate
    int16_t speed_meas = motorSpeed_get();

    // calculate control error
    int16_t speed_error = speed_ref - speed_meas;
    q22_10_t speed_error_q16_16 = INT_TO_Q22_10(speed_error);

    // calculate PI control
    q22_10_t controller_P = q22_10_mul(speed_error_q16_16,Kp);
    integrator_speed += q22_10_mul(speed_error_q16_16,Ts_controller);
    q22_10_t controller_I = q22_10_mul(integrator_speed,Ki);
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

void torque_controller_step() {
    if (motorCurrent_get == NULL || dutyCycle_set == NULL) return;

    // measured current
    q4_12_t current_meas = motorCurrent_get();

    // calculate desired current using torque ref
    q4_12_t current_ref = q4_12_div(torque_ref, K_times_Psi_q4_12);

    // calculate control error
    q4_12_t current_error = current_ref - current_meas;
    q22_10_t current_error_q22_10 = Q4_12_TO_Q22_10(current_error);

    // calculate PI control
    q22_10_t controller_P = q22_10_mul(current_error_q22_10,Kpt);
    integrator_current += q22_10_mul(current_error_q22_10,Ts_controller);
    q22_10_t controller_I = q22_10_mul(integrator_current,Kit);
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

void resetSpeedIntegrator() {
    integrator_speed = INT_TO_Q22_10(0);
}
void resetCurrentIntegrator() {
    integrator_current = INT_TO_Q22_10(0);
}