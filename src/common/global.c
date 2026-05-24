#include <stdint.h>

#include "fixed_point.h"
#include "math.h"
#include "global.h"

const uint8_t init_ticks = 1u;

const q22_10_t Ts_controller = FLOAT_TO_Q22_10(0.01f); // 10ms control time step
const int16_t max_speed = 10000u;

const uint8_t max_current = 4u;

const q22_10_t one_q16_16 = FLOAT_TO_Q22_10(1.0f);
const q22_10_t zero_q16_16 = FLOAT_TO_Q22_10(0.0f);

const float K_times_Psi = 0.01060178378f; // 0.0196133 Nm at 1.85A stall current
const q4_12_t K_times_Psi_q4_12 = FLOAT_TO_Q4_12(K_times_Psi);

// duty parameters
const int8_t duty_count = 200u;
const int8_t duty_mean = 100u;
const int8_t duty_min = -96;
const int8_t duty_max = 96;
