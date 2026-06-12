#include <stdint.h>

#include "test_abstraction.h"
#include "fixed_point.h"
#include "motor.h"

int16_t calculateSpeed() {
    int16_t speed = (int16_t) get_motor_speed_rpm();
    return speed;
}

q4_12_t measureCurrent() {
    q4_12_t current = FLOAT_TO_Q4_12(get_motor_current());
    return current;
}

int16_t get_motor_speed_est() {
    int16_t speed = (int16_t) get_motor_speed_rpm();
    return speed;
}

void enable_pwm(void) {}
void disable_pwm(void) {}
void set_duty_cycle(q8_8_t duty_in) {}
