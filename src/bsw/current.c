
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

volatile uint16_t leftADCticks = 0u;
volatile uint16_t rightADCticks = 0u;

q4_12_t measureCurrent() {
    // TODO: compare to q8_8 scaled value?
    if (duty >= 0) {
        // positive current
        return ((q4_12_t) (rightADCticks * adcScaleFactorDiscrete));
    }
    else {
        // negative current, invert value
        // TODO: invert value when negative duty
        return -((q4_12_t) (leftADCticks * adcScaleFactorDiscrete));
    }
}
