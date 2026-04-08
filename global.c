#include "fixed_point.h"
#include "math.h"

const float Ts_encoder = 0.001f;
const unsigned int controller_run_ticks = 10u;
const float encoder_LP_float = 0.015625f; //0.01f;
const unsigned int init_ticks = 100u;

const q8_8_t Ts_controller = FLOAT_TO_Q8_8(0.01f);
const q10_6_t encoder_LP = FLOAT_TO_Q10_6(encoder_LP_float);
const q10_6_t encoder_LP2 = FLOAT_TO_Q10_6(1.0f - encoder_LP_float);
const q10_6_t max_speed = FLOAT_TO_Q10_6(100.0f);

const q8_8_t one_q8_8 = FLOAT_TO_Q8_8(1.0f);
const q8_8_t zero_q8_8 = FLOAT_TO_Q8_8(0.0f);
const q10_6_t zero_q10_6 = FLOAT_TO_Q10_6(0.0f);
const q10_6_t speed_jump = FLOAT_TO_Q10_6( (M_PI / 3.0f) / Ts_encoder);
const q10_6_t speed_jump_scaled = FLOAT_TO_Q10_6( (M_PI / 3.0f) / Ts_encoder * encoder_LP_float);

//char testchar = 0u;
//int testint = 0;
//short int testshort = 0;