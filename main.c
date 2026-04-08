#include <stdio.h>

#include "controller.h"
#include "global.h"
#include "motor.h"
#include "encoder.h"
#include "system.h"
#include "fixed_point.h"

int main() {

    // parameters for simulation
    float uBat = 12.0f;
    float speed_ref = 50.0f;
    q10_6_t speed_ref_discrete = FLOAT_TO_Q10_6(speed_ref);
    q8_8_t uBat_discrete = FLOAT_TO_Q8_8(uBat);

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
        run_system(speed_ref_discrete, uBat_discrete);

        // simulate motor
        step_motor(uBat);

        // log data to csv
        float time = Ts_encoder * (float) i;
        float speed = get_motor_speed();
        float speed_est = Q10_6_TO_FLOAT(get_motor_speed_est_discrete());
        float motor_angle = get_motor_angle();
        float current = get_motor_current();
        q8_8_t duty_discrete = get_motor_duty();
        float duty = Q8_8_TO_FLOAT(duty_discrete);
        controller_state_t state = get_controller_state();
        fprintf(fptr,"%f,%f,%f,%f,%f,%f,%d\n",time,speed,speed_est,motor_angle,current,duty,state);
    }
    fclose(fptr);

    return 0;
}