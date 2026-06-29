#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "controller.h"
#include "global.h"
#include "motor.h"
#include "test_abstraction.h"
#include "run_simulation.h"
#include "system.h"
#include "fixed_point.h"

uint8_t modeReq = STATE_SPEED;
static bool powerStageReq = false;
static int16_t speedRef = 0;
static q4_12_t torqueRef = 0;
static q8_8_t dutyRef = 0;

static float time = 0.0f;

uint8_t returnDiagModeRequest() {
    return modeReq;
};
bool returnDiagPowerStageRequest() {
    return powerStageReq;
}
q4_12_t returnDiagTorqueRequest() {
    return torqueRef;
}
int16_t returnDiagSpeedRequest() {
    return speedRef;
}
q8_8_t returnDiagPWMRequest() {
    return dutyRef;
}

int main() {

    // parameters for simulation
    float uBat = 12.0f;
    float speedSlope = 10000.0f;

    //q22_10_t uBat_discrete = FLOAT_TO_Q22_10(uBat);

    // prepare file for logging
    FILE *fptr;
    fptr = fopen("motor_simulation.csv","w");
    if (fptr == NULL) {
        printf("file could not be opened!\n");
        return 1;
    }
    fprintf(fptr,"time,torque,speed,motor_angle,current,duty,state,mode\n");

    // run simulation for 1s, time step 1ms
    uint16_t count_controller = 10u;
    for(uint16_t i=0;i<1000;i++) {

        // step controller at 100Hz
        if (count_controller == 10u) {
            run_system();
            count_controller = 0u;
        }
        count_controller += 1u;

        // simulate motor
        step_motor(uBat, speedSlope);

        // update time counter
        time = Ts * (float) i;

        // if time > 0.1s, activate power stage
        if (time > 0.1f) {
            powerStageReq = true;
        }
        // if time > 0.2s, set request
        if (time > 0.2f) {
            speedRef = 1000;
        }
        // if time > 0.4s, reset request
        if (time > 0.4f) {
            speedRef = 0;
        }
        // if time > 0.6s, go to torque request
        if (time > 0.6f) {
            modeReq = STATE_TORQUE;
        }
        // if time > 0.8s, set  torque request
        if (time > 0.8f) {
            torqueRef = FLOAT_TO_Q4_12(0.01f);
        }

        // log data to csv
        float speed = get_motor_speed_rpm();
        float motor_angle = get_motor_angle();
        float current = get_motor_current();
        float torque = get_motor_torque();
        float duty = Q8_8_TO_FLOAT(get_motor_duty());
        controller_state_t state = get_controller_state();
        controller_mode_t mode = get_controller_mode();
        fprintf(fptr,"%f,%f,%f,%f,%f,%f,%d,%d\n",time,torque,speed,motor_angle,current,duty,state,mode);
    }
    fclose(fptr);

    return 0;
}