
#include "global.h"
#include "motor.h"
#include "stdbool.h"
#include "controller.h"
#include "fixed_point.h"
#include "run_simulation.h"

#include <math.h>

// assumed
static const float B = 0.001f;

// ts
const float Ts = 0.001f;

// data sheet constants
static const float R = 4.2f; //4.2
static const float J = 0.9E-6f; // 0.9E-6 kgm2 rotor inertia
static const float Ki_times_Psi = 0.01432394487f; // 8000 rpm no load speed at 12V
static const float L = 0.32e-3f; // stator inductance 0.32 mH

// motor states
static float motor_speed = 0.0f; // rad/s
static float motor_speed_rpm = 0.0f; // rpm
static float motor_angle = 0.0f;
static float torque = 0.0f;
static float current = 0.0f;

void step_motor(float uBat, float speedSlope) {
    // previous current
    static float i_prev = 0.0f;

    // speed is controlled using torque
    if (speedModeReq) {
        // get controller duty cycle and calculate motor voltage
        int16_t duty_discrete = get_motor_duty();
        float voltage = (float) duty_discrete / (float) duty_mean * uBat;

        // calculate motor driving voltage
        float U_ind = Ki_times_Psi * motor_speed;

        // calculate current and torque
        current = (voltage - U_ind - L * (current - i_prev) / Ts) / R;
        torque = K_times_Psi * current;

        // update motor angle and speed and current
        float angular_acc = (torque - 0.0f * motor_speed) / J;
        motor_speed += angular_acc * Ts;
        motor_angle += motor_speed * Ts;
        motor_angle = fmodf(motor_angle, 2.0f * M_PI);
        motor_speed_rpm = motor_speed * 30.0f / M_PI;
    }
    // torque is controlled using current, speed is fixed
    else {
        motor_speed_rpm += speedSlope * Ts;
        if (motor_speed_rpm > 1000.0f) {
            motor_speed_rpm = 1000.0f;
        }
        motor_speed = motor_speed_rpm * M_PI / 30.0f;
        motor_angle += motor_speed * Ts;
        motor_angle = fmodf(motor_angle, 2.0f * M_PI);

        // get controller duty cycle and calculate motor voltage
        int16_t duty_discrete = get_motor_duty();
        float voltage = (float) duty_discrete / (float) duty_mean * uBat;

        // calculate motor driving voltage
        float U_ind = Ki_times_Psi * motor_speed;

        // calculate current and torque
        current = (voltage - U_ind - L * (current - i_prev) / Ts) / R;
        torque = K_times_Psi * current;
    }

    // prev current
    i_prev = current;
}

float get_motor_speed_rpm() {
    return motor_speed_rpm;
}

float get_motor_angle() {
    return motor_angle;
}

float get_motor_current() {
    return current;
}

float get_motor_torque() {
    return torque;
}