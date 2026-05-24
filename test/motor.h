
#include <stdbool.h>

// ts
extern const float Ts;
extern const q4_12_t K_times_Psi_q4_12;

void step_motor(float, float);

float get_motor_speed_rpm(void);

float get_motor_angle(void);

float get_motor_current(void);

float get_motor_torque(void);