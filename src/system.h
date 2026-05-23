#include <stdint.h>

#include "fixed_point.h"

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR
} controller_state_t;

void state_actions(int16_t speed_ref);
void check_transitions(void);
void entry_actions(void);
void run_system(int16_t speed_ref);

controller_state_t get_controller_state(void);