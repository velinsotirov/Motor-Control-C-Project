#include <stdio.h>

#include "controller.h"
#include "global.h"
#include "motor.h"
#include "encoder.h"
#include "system.h"

int main() {

    // parameters for simulation
    float uBat = 12.0f;
    float speed_ref = 50.0f;

    // prepare file for logging
    FILE *fptr;
    fptr = fopen("motor_simulation.csv","w");
    if (fptr == NULL) {
        printf("file could not be opened!\n");
        return 1;
    }
    fprintf(fptr,"time,speed,speed_est,motor_angle,current,duty,state\n");

    // test sizes
    //printf("%zu\n", sizeof(testchar));
    //printf("%zu\n", sizeof(testint));
    //printf("%zu\n", sizeof(testshort));

    // run simulation for 1s
    for(unsigned int i=0;i<1001;i++) {

        // step controller at 1kHz
        run_system(speed_ref, uBat);

        // simulate motor
        step_motor(uBat);

        // log data to csv
        float time = Ts_encoder * (float) i;
        float speed = get_motor_speed();
        float speed_est = get_motor_speed_est();
        float motor_angle = get_motor_angle();
        float current = get_motor_current();
        float duty = get_motor_duty();
        controller_state_t state = get_controller_state();
        fprintf(fptr,"%f,%f,%f,%f,%f,%f,%d\n",time,speed,speed_est,motor_angle,current,duty,state);
    }
    fclose(fptr);

    return 0;
}