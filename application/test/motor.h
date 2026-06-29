
#include <stdbool.h>

// ts
extern const float Ts;

void step_motor(float, float);

float get_motor_speed_rpm(void);

float get_motor_angle(void);

float get_motor_current(void);

float get_motor_torque(void);