
#include "fixed_point.h"

int16_t calculateSpeed(void);
q4_12_t measureCurrent(void);
int16_t get_motor_speed_est(void);
void enable_pwm(void);
void disable_pwm(void);
void set_duty_cycle(int16_t);
