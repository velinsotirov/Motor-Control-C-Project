
#include <stdint.h>

#include "system.h"
#include "controller.h"
#include "motor.h"
#include "global.h"
#include "fixed_point.h"

#if defined(TEST_BUILD)
#include "test_abstraction.h"
#else
#include "encoder.h"
#endif

static controller_state_t controller_state = STATE_IDLE;
static controller_state_t controller_prev_state = STATE_IDLE;

static uint8_t idle_counter = 0;

// run entire FSM
void run_system(int16_t speed_ref) {
    check_transitions();
    entry_actions();
    state_actions(speed_ref);
}

void check_transitions() {
    switch (controller_state) {
        // transitions from idle
        case STATE_IDLE:
            // go to running after certain time
            if (idle_counter == init_ticks) {
                controller_state = STATE_RUNNING;
            }
            break;
        // transitions from running
        case STATE_RUNNING:
            int16_t current_speed = get_motor_speed_est();
            int8_t measuredCurrent = 0;
            // go to error if we have a high speed or overcurrent
            if ((current_speed > max_speed || current_speed < -max_speed) || \
            (measuredCurrent > max_current || measuredCurrent < -max_current)) {
                controller_state = STATE_ERROR;
            }
            break;
        case STATE_ERROR:
            // exit from error requires HW reset
            break;
    }
}

void entry_actions() {
    if (controller_state != controller_prev_state) {
        switch (controller_state) {
            case STATE_IDLE:
                // reset idle counter
                idle_counter = 0;
                break;
            case STATE_RUNNING:
                // attach controller to motor and reset idle counter
                attach_controller(calculateSpeed, set_motor_duty);
                idle_counter = 0;
                break;
            case STATE_ERROR:
                // detach controller, duty cycle is reset within
                detach_controller();
                break;
        }
    }
}

void state_actions(int16_t speed_ref) {
    switch (controller_state) {
        case STATE_IDLE:
            // increment counter so we can transition to running when time expires
            idle_counter++;
            break;
        case STATE_RUNNING:
            // run controller
            // controller fetches current speed estimate by itself, so no encoder call necessary
            controller_update(speed_ref);
            break;
        case STATE_ERROR:
            // do nothing for now
            break;
        default:
            break;
    }
}

controller_state_t get_controller_state() {
    return controller_state;
}