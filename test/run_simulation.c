#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "controller.h"
#include "global.h"
#include "motor.h"
#include "test_abstraction.h"
#include "system.h"
#include "fixed_point.h"

int main() {

    // parameters for simulation
    float uBat = 12.0f;
    bool speed_mode = false;
    int16_t speed_ref = 1000;
    float speedSlope = 10000.0f;
    q4_12_t torque_ref = FLOAT_TO_Q4_12(0.01f); // approx. half of max torque

    //q22_10_t uBat_discrete = FLOAT_TO_Q22_10(uBat);

    // prepare file for logging
    FILE *fptr;
    fptr = fopen("motor_simulation.csv","w");
    if (fptr == NULL) {
        printf("file could not be opened!\n");
        return 1;
    }
    fprintf(fptr,"time,torque,speed,motor_angle,current,duty,state\n");

    // run simulation for 1s, time step 1ms
    uint16_t count_controller = 10u;
    for(uint16_t i=0;i<1000;i++) {

        // step controller at 100Hz
        if (count_controller == 10u) {
            run_system(speed_mode, torque_ref, speed_ref);
            count_controller = 0u;
        }
        // simulate motor
        step_motor(speed_mode, uBat, speedSlope);

        // increment counter
        count_controller += 1u;

        // log data to csv
        float time = Ts * (float) i;
        float speed = get_motor_speed_rpm();
        float motor_angle = get_motor_angle();
        float current = get_motor_current();
        float torque = get_motor_torque();
        int8_t duty_discrete = get_motor_duty();
        controller_state_t state = get_controller_state();
        fprintf(fptr,"%f,%f,%f,%f,%f,%d,%d\n",time,torque,speed,motor_angle,current,duty_discrete,state);
    }
    fclose(fptr);

    return 0;
}