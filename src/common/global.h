#ifndef GLOBAL_H
#define GLOBAL_H

#include "fixed_point.h"

extern const float Ts_encoder;
extern const float encoder_LP_float;
extern const unsigned int init_ticks;
extern const unsigned int controller_run_ticks;

extern const q8_8_t Ts_controller;
extern const q10_6_t encoder_LP;
extern const q10_6_t encoder_LP2;
extern const q10_6_t max_speed;

extern const q8_8_t one_q8_8;
extern const q8_8_t zero_q8_8;
extern const q10_6_t zero_q10_6;
extern const q10_6_t speed_jump;
extern const q10_6_t speed_jump_scaled;
//extern char testchar;
//extern int testint;
//extern short int testshort;

#endif // GLOBAL_H