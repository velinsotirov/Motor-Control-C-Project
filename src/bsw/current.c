
#include <stdint.h>
#include <stdbool.h>

#include "current.h"
#include "controller.h"
#include "fixed_point.h"

// current measurement constants
// we have 0.5Ohm shunt and no OP AMP. assuming 1A flow, shunt voltage is 0.5V, which means 620.45 ADC ticks
// therefore, to get the 1A current using the ticks, we have to multiply with 1/620.45 = 0.00161173341
static const q4_12_t adcScaleFactorDiscrete = FLOAT_TO_Q4_12(0.00161173341f); // 6% error caused by quantization!

volatile uint16_t leftADCticks = 0u;
//volatile uint16_t rightADCticks = 0u;

q4_12_t measureCurrent() {
    // TODO: compare to q8_8 scaled value?
    if (duty >= 0) {
        // positive current
        return ((q4_12_t) (leftADCticks * adcScaleFactorDiscrete));
    }
    else {
        // negative current, invert value
        // TODO: invert value when negative duty
        return -((q4_12_t) (leftADCticks * adcScaleFactorDiscrete));
    }
}
