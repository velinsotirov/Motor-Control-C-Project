
#include <stdint.h>
#include <stdbool.h>

#include "system.h"
#include "controller.h"
#include "global.h"
#include "fixed_point.h"

#if defined(TEST_BUILD)
#include "test_abstraction.h"
#include "run_simulation.h"
#else
#include "encoder.h"
#include "current.h"
#include "diag.h"
#endif

#ifdef __AVR_ATmega328P__
#include "atmega328p_pwm.h"
#elif defined(__ARM_CortexM3__)
#include "stm32_pwm.h"
#endif

// states are hidden from other modules
static controller_state_t controller_state = STATE_IDLE;
static controller_state_t controller_prev_state = STATE_IDLE;
static controller_mode_t controller_mode = STATE_TORQUE;
static controller_mode_t controller_prev_mode = STATE_TORQUE;

// internal counter
static uint8_t idle_counter = 0;

// bugfix
static int16_t current_speed = 0;
static int8_t measuredCurrent = 0;

// system state requests, controller accesses them directly
// changed by diag module when outside requests arrive
bool speed_mode = false;
bool powerstage_mode = false;
q4_12_t torque_ref = 0;
int16_t speed_ref = 0;

void check_transitions() {
    switch (controller_state) {
        // transitions from idle
        case STATE_IDLE:
            // go to running if powerstage is requested and we have been in idle for some time
            if (powerstage_mode && idle_counter > 10) {
                controller_state = STATE_RUNNING;
            }
            break;
        // transitions from running
        case STATE_RUNNING:
            current_speed = get_motor_speed_est();
            measuredCurrent = 0;
            // go to error if we have a high speed or overcurrent
            if ((current_speed > max_speed || current_speed < -max_speed) || \
            (measuredCurrent > max_current || measuredCurrent < -max_current)) {
                controller_state = STATE_ERROR;
            }

            if (!powerstage_mode) {
                controller_state = STATE_IDLE;
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
                // reset idle counter and detach controller
                // duty cycle and integrators are reset within detach function
                detach_controller(); // causes a HARD FAULT if PWM is not initialized!
                idle_counter = 0;
                break;
            case STATE_RUNNING:
                // attach controller to motor and reset idle counter
                attach_controller(calculateSpeed, measureCurrent, set_duty_cycle);
                idle_counter = 0;
                break;
            case STATE_ERROR:
                // detach controller, duty cycle and integrators are reset within detach function
                detach_controller();
                break;
        }
    }
}

void check_subtransitions() {
    switch (controller_mode) {
        case STATE_TORQUE:
            if (speed_mode) {
                controller_mode = STATE_SPEED;
            }
            break;
        case STATE_SPEED:
            if (!speed_mode) {
                controller_mode = STATE_TORQUE;
            }
            break;
    }
}

void entry_subactions() {
    if (controller_mode != controller_prev_mode) {
        switch (controller_mode) {
            case STATE_TORQUE:
                // when we enter torque, reset speed integrator
                resetSpeedIntegrator();
                break;
            case STATE_SPEED:
                // when we enter speed, reset current integrator
                resetCurrentIntegrator();
                break;
        }
    }
}

void state_subactions() {
    switch (controller_mode) {
        // run controller
        case STATE_SPEED:
            speed_controller_step();
            break;
        case STATE_TORQUE:
            torque_controller_step();
            break;
    }
}

void state_actions() {
    switch (controller_state) {
        case STATE_IDLE:
            // increment counter so we can transition to running when time expires
            idle_counter++;
            break;
        case STATE_RUNNING:
            // transition between torque and speed mode
            check_subtransitions();
            entry_subactions();

            // running controller in torque or speed mode
            state_subactions();
            break;
        case STATE_ERROR:
            // do nothing for now
            break;
        default:
            break;
    }
}

// run entire FSM
void run_system() {
    // save state and mode before any transitions, so we detect changes
    controller_prev_state = controller_state;
    controller_prev_mode = controller_mode;

    // update system requests using diag requests
    speed_mode = returnDiagModeRequest();
    powerstage_mode = returnDiagPowerStageRequest();
    torque_ref = returnDiagTorqueRequest();
    speed_ref = returnDiagSpeedRequest();

    // run FSM
    check_transitions();
    entry_actions();
    state_actions();
}

controller_state_t get_controller_state() {
    return controller_state;
}
controller_mode_t get_controller_mode() {
    return controller_mode;
}