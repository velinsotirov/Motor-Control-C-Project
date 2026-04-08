#include "global.h"
#include "motor.h"
#include "math.h"

// states
static unsigned char encoder_state = 0u;
static float motor_speed_est = 0.0f;

void encoder_state_update(void) {
    float motor_angle = get_motor_angle();
    if (motor_angle < M_PI / 3.0f) {
        encoder_state = 0u;
    } else if (motor_angle < M_PI * (2.0f/3.0f)) {
        encoder_state = 1u;
    } else if (motor_angle < M_PI) {
        encoder_state = 2u;
    } else if (motor_angle < M_PI * (4.0f/3.0f)) {
        encoder_state = 3u;
    } else if (motor_angle < M_PI * (5.0f/3.0f)) {
        encoder_state = 4u;
    } else {
        encoder_state = 5u;
    }
}

void speed_estimator(void) {

    // previous encoder state
    static unsigned char prev_encoder_state = 0u;

    // local var
    float speed_estimate = 0.0f;

    // calculate speed estimate based on encoder state change
    if ((encoder_state > prev_encoder_state) || (encoder_state == 0u && prev_encoder_state == 5u)) {
        speed_estimate = (M_PI / 3.0f) / Ts_encoder;
    } else if ((encoder_state < prev_encoder_state) || (encoder_state == 5u && prev_encoder_state == 0u)) {
        speed_estimate = -(M_PI / 3.0f) / Ts_encoder;
    } else {
        speed_estimate = 0.0f;
    }

    // update speed estimate
    motor_speed_est = (1.0f - encoder_LP) * motor_speed_est + encoder_LP * speed_estimate;

    // update previous encoder state
    prev_encoder_state = encoder_state;

}

float get_motor_speed_est(void) {  
    return motor_speed_est;
}