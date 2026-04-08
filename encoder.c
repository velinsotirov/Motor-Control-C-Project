#include "global.h"
#include "motor.h"
#include "math.h"
#include "fixed_point.h"

// states
static unsigned char encoder_state = 0u;
static q10_6_t motor_speed_est = 0;

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
    q10_6_t speed_estimate = zero_q10_6;

    // calculate speed estimate based on encoder state change
    if ((encoder_state > prev_encoder_state) || (encoder_state == 0u && prev_encoder_state == 5u)) {
        //speed_estimate = speed_jump;
        speed_estimate = speed_jump_scaled;
    } else if ((encoder_state < prev_encoder_state) || (encoder_state == 5u && prev_encoder_state == 0u)) {
        //speed_estimate = -speed_jump;
        speed_estimate = -speed_jump_scaled;
    } else {
        speed_estimate = zero_q10_6;
    }

    // update speed estimate
    //motor_speed_est = q10_6_mul(encoder_LP2, motor_speed_est) + q10_6_mul(encoder_LP, speed_estimate);
    motor_speed_est = q10_6_mul(encoder_LP2, motor_speed_est) + speed_estimate;


    // update previous encoder state
    prev_encoder_state = encoder_state;

}

q10_6_t get_motor_speed_est_discrete(void) {  
    return motor_speed_est;
}