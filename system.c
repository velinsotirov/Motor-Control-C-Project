#include "system.h"
#include "encoder.h"
#include "controller.h"
#include "motor.h"
#include "global.h"

static controller_state_t controller_state;

void run_system(float speed_ref, float uBat) {
    static int idle_counter = 0;
    static unsigned char run_counter = 1u;
    switch (controller_state) {
        case STATE_IDLE:
            idle_counter++;

            // wait 0.1s before starting controller
            if (idle_counter > init_ticks) {
                controller_state = STATE_RUNNING;

                // attach controller to motor
                attach_controller(get_motor_speed_est,set_motor_duty);
                idle_counter = 0;
            }
            break;
        case STATE_RUNNING:
            // run encoder and speed estimator at 1kHz
            encoder_state_update();
            speed_estimator();

            // run controller at 100Hz
            if (run_counter == controller_run_ticks) {
                controller_update(speed_ref,uBat);
                run_counter = 0;
            }
            run_counter++;

            // check for error conditions
            if (get_motor_speed_est() > max_speed || get_motor_current() > max_current) {
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