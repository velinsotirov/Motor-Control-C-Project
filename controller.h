#include <math.h>

void controller_update(float, float);

typedef float (*GetMotorSpeed) (void);

typedef void (*SetDuty) (float);

void attach_controller(GetMotorSpeed, SetDuty);

void detach_controller(void);

void set_motor_duty(float);

float get_motor_duty(void);

void reset_duty(void);