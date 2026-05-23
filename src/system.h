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
    STATE_TORQUE
} controller_mode_t;

extern bool speed_mode;
extern q4_12_t torque_ref;
extern int16_t speed_ref;

void run_system(bool speedMode, q4_12_t torqueRef, int16_t speedRef);

controller_state_t get_controller_state(void);