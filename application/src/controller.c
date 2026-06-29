#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include "global.h"
#include "system.h"
#include "controller.h"
#include "fixed_point.h"

#ifdef __AVR_ATmega328P__
#include "atmega328p_pwm.h"
//16MHz / (256 * (124 + 1)) -> 500Hz, we want 100Hz so flag is 5
#define CONTROLLER_COUNT 5u
#elif defined(__ARM_CortexM3__)
#include "stm32_pwm.h"
// systick is 1000Hz, so counter value is twice as large
#define CONTROLLER_COUNT 10u
#elif defined(TEST_BUILD)
#include "test_abstraction.h"
// unused value, added to avoid errors
#define CONTROLLER_COUNT 0u
#endif

#define KP_FLOAT_VAL 0.01f
#define KPT_FLOAT_VAL 5.0f
#define KI_FLOAT_VAL 0.5f
#define KIT_FLOAT_VAL 2000.0f

// when counter has moved this much, its time to execute the controller
const uint16_t t_step_controller = CONTROLLER_COUNT;

// duty output from controller, with initial condition
q8_8_t duty = 0;

// speed controller constants
static const q22_10_t Kp_speed = FLOAT_TO_Q22_10(KP_FLOAT_VAL);
static const q22_10_t Ki_speed = FLOAT_TO_Q22_10(KI_FLOAT_VAL);

// torque controller constants
static const q16_16_t Kp_current = FLOAT_TO_Q16_16(KPT_FLOAT_VAL);
static const q16_16_t Ki_current = FLOAT_TO_Q16_16(KIT_FLOAT_VAL);

// speed controller states, static for file so they can be reset when detaching
static q22_10_t integrator_speed = 0;
static q16_16_t integrator_current = 0;

// function pointers
static GetMotorSpeed motorSpeed_get = NULL;
static GetMotorCurrent motorCurrent_get = NULL;
static SetDuty dutyCycle_set = NULL;

void attach_controller(GetMotorSpeed s, GetMotorCurrent c, SetDuty d) {
    motorSpeed_get = s;
    motorCurrent_get = c;
    dutyCycle_set = d;
    enable_pwm();
}

void detach_controller(void) {
    disable_pwm();
    reset_duty();
    motorSpeed_get = NULL;
    motorCurrent_get = NULL;
    dutyCycle_set = NULL;
    integrator_speed = FLOAT_TO_Q22_10(0.0f);
    integrator_current = FLOAT_TO_Q16_16(0.0f);
}

void speed_controller_step() {
    if (motorSpeed_get == NULL || dutyCycle_set == NULL) return;

    // get encoder speed estimate
    int16_t speed_meas = 0; // motorSpeed_get(); TODO: activate once speed measurement works!

    // calculate control error
    int16_t speed_error = speed_ref - speed_meas;
    q22_10_t speed_error_q16_16 = INT_TO_Q22_10(speed_error);

    // calculate PI control
    q22_10_t controller_P = q22_10_mul(speed_error_q16_16,Kp_speed);
    integrator_speed += q22_10_mul(speed_error_q16_16,Ts_controller_q22_10);
    q22_10_t controller_I = q22_10_mul(integrator_speed,Ki_speed);
    q8_8_t duty_unlim = Q22_10_TO_Q8_8(controller_P + controller_I);

    // limit duty cycle to [-85,-15] and [15,85]
    q8_8_t duty_lim = limitDutyCycle(duty_unlim);

    // TODO: attach once speed measurement works!
    duty = INT_TO_Q8_8(0); //duty_lim;

    // set duty cycle
    dutyCycle_set(duty);
}

void torque_controller_step() {
    if (motorCurrent_get == NULL || dutyCycle_set == NULL) return;

    // measured current
    q4_12_t current_meas = 0u; // motorCurrent_get(); TODO: activate once current measurement works!

    // calculate desired current using torque ref
    q4_12_t current_ref = q4_12_div(torque_ref, K_times_Psi_q4_12);
    
    // calculate control error
    q4_12_t current_error = current_ref - current_meas;
    q16_16_t current_error_q16_16 = Q4_12_TO_Q16_16(current_error);

    // calculate PI control
    q16_16_t controller_P = q16_16_mul(current_error_q16_16,Kp_current);
    integrator_current += q16_16_mul(current_error_q16_16,Ts_controller_q16_16);
    q16_16_t controller_I = q16_16_mul(integrator_current,Ki_current);
    q8_8_t duty_unlim = Q16_16_TO_Q8_8(controller_P + controller_I);
    
    // limit duty cycle to [-85,-15] and [15,85]
    q8_8_t duty_lim = limitDutyCycle(duty_unlim);

    // TODO: attach once current measurement works!
    duty = INT_TO_Q8_8(0u); //duty_lim;

    // set duty cycle
    dutyCycle_set(duty);
}

void pwm_controller_step() {
    q8_8_t duty_unlim = duty_ref;

    // limit duty cycle to [-85,-15] and [15,85]
    q8_8_t duty_lim = limitDutyCycle(duty_unlim);

    // send out command
    duty = duty_lim;

    // set duty cycle
    dutyCycle_set(duty);
}

q8_8_t limitDutyCycle(q8_8_t duty_unlim) {
    if (duty_unlim == 0) {
        return duty_unlim;
    }
    else if (duty_unlim > 0) {
        if (duty_unlim > duty_max) {
            duty_unlim = duty_max;
        }
        else if (duty_unlim < duty_min) {
            duty_unlim = duty_min;
        }
    }
    else {
        if (duty_unlim < -duty_max) {
            duty_unlim = -duty_max;
        }
        else if (duty_unlim > -duty_min) {
            duty_unlim = -duty_min;
        }
    }
    return duty_unlim;
}

q8_8_t get_motor_duty(void) {
    return duty;
}

void reset_duty(void) {
    duty = 0u;
    set_duty_cycle(duty);
}

void resetSpeedIntegrator() {
    integrator_speed = INT_TO_Q22_10(0);
}
void resetCurrentIntegrator() {
    integrator_current = INT_TO_Q16_16(0);
}