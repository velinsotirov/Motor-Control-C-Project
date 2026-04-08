#include "fixed_point.h"

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR
} controller_state_t;

void run_system(q10_6_t speed_ref, q8_8_t uBat_discrete);

controller_state_t get_controller_state(void);