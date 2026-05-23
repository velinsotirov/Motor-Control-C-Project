#include <stdint.h>

#include "test_abstraction.h"
#include "motor.h"

int16_t calculateSpeed() {
    int16_t speed = (int16_t) get_motor_speed_rpm();
    return speed;
}

int16_t get_motor_speed_est() {
    int16_t speed = (int16_t) get_motor_speed_rpm();
    return speed;
}