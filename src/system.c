
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

void run_system(int16_t speed_ref) {
    static uint8_t idle_counter = 0;
    switch (controller_state) {
        case STATE_IDLE:
            idle_counter++;

            // wait a bit before starting controller
            if (idle_counter == init_ticks) {
                controller_state = STATE_RUNNING;

                // attach controller to motor
                attach_controller(calculateSpeed, set_motor_duty);
                idle_counter = 0;
            }
            break;
        case STATE_RUNNING:
            // run controller
            // controller fetches current speed estimate by itself, so no encoder call necessary
            controller_update(speed_ref);

            // update duty cycle
            pwmCommands();

            // check for error conditions
            if (get_motor_speed_est() > max_speed) {
                controller_state = STATE_ERROR;
                detach_controller();
            }
            break;
        case STATE_ERROR:
        default:
            break;
    }
}

controller_state_t get_controller_state() {
    return controller_state;
}