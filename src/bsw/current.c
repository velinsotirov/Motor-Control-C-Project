
#include <stdint.h>
#include <stdbool.h>

#include "current.h"
#include "controller.h"
#include "fixed_point.h"

// current measurement constants
//const float ampGain = 18.0f;
//const float shuntR = 0.5f;
//const float voltageGain = (float) 5 / (float) 1023;
// adcScaleFactor = voltageGain/(ampGain*shuntR)
//const float adcScaleFactor = 0.00054306505f;
//measuredCurrent = (float) (currentADCticks*voltageGain/(ampGain*shuntR));

static const q4_12_t adcScaleFactorDiscrete = FLOAT_TO_Q4_12(0.00054306505f); //10% error!

volatile uint16_t currentADCticks = 0u; 

q4_12_t measureCurrent() {
    if (duty >= 0) {
        // positive current
        return q4_12_mul(currentADCticks, adcScaleFactorDiscrete);
    }
    else {
        // negative current, invert value
        return q4_12_mul((~currentADCticks)+1u, adcScaleFactorDiscrete);
    }
}
