#include "global.h"
#include "controller.h"

#include <math.h>

// constants
static const float K = 100.0f;
static const float R = 0.3f;
static const float psi = 0.0005f;
static const float J = 0.0001f;
static const float B = 0.001f;

// motor states
static float motor_speed = 0.0f;
static float motor_angle = 0.0f;
static float torque = 0.0f;
static float current = 0.0f;

void step_motor(float uBat) {

    // get controller duty cycle and calculate motor voltage
    float duty = get_motor_duty();
    float voltage = duty * uBat;

    // calculate motor driving voltage
    float U_ind = K*psi*motor_speed;

    // calculate current and torque
    current = (voltage - U_ind)/R;
    torque = K*psi*current;

    // update motor angle and speed
    float angular_acc = (torque - B * motor_speed)/J;
    motor_speed += angular_acc*Ts_encoder;
    motor_angle += motor_speed*Ts_encoder;
    motor_angle = fmodf(motor_angle, 2.0f * M_PI);

}

float get_motor_speed(void) {
    return motor_speed;
}

float get_motor_angle(void) {
    return motor_angle;
}

float get_motor_current(void) {
    return current;
}