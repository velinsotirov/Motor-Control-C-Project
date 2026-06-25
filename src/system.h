#include <stdint.h>
#include <stdbool.h>

#include "fixed_point.h"

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR
} controller_state_t;

typedef enum {
    STATE_SPEED,
    STATE_TORQUE,
    STATE_PWM
} controller_mode_t;

extern bool speed_mode;
extern q4_12_t torque_ref;
extern int16_t speed_ref;
extern q8_8_t duty_ref;

void run_system();

controller_state_t get_controller_state(void);
controller_mode_t get_controller_mode(void);