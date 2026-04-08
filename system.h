typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR
} controller_state_t;

void run_system(float, float);

controller_state_t get_controller_state(void);