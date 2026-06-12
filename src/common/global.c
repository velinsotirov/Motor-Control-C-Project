
#include <stdint.h>

#include "math.h"
#include "global.h"
#include "fixed_point.h"

// 0.0196133 Nm at 1.85A stall current
#define K_TIMES_PSI_VAL 0.01060178378f

const q22_10_t Ts_controller_q22_10 = FLOAT_TO_Q22_10(0.01f); // 10ms control time step
const q16_16_t Ts_controller_q16_16 = FLOAT_TO_Q16_16(0.01f);
const int16_t max_speed = 10000u;

const uint8_t max_current = 4u;

const float K_times_Psi = K_TIMES_PSI_VAL;
const q4_12_t K_times_Psi_q4_12 = FLOAT_TO_Q4_12(K_TIMES_PSI_VAL);

// duty parameters
const int8_t duty_mean = 100u;
const q8_8_t duty_min = INT_TO_Q8_8(-94);
const q8_8_t duty_max = INT_TO_Q8_8(94);
