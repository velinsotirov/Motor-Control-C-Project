#include <stdint.h>

#include "fixed_point.h"

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR
} controller_state_t;

void run_system(int16_t speed_ref);

controller_state_t get_controller_state(void);