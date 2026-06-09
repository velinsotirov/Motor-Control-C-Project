
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "current.h"
#include "controller.h"
#include "atmega328p_adc.h"

void setupADC() {
    ADMUX = 0b0;
    //pins REFS1, REFS0 = 0 -> we use AREF!
    //pins MUX3,2,1,0 0000 for ADC0, we will toggle
    //between ADC1 and ADC0 depending on current direction

    ADCSRA = 0b0;
    SET(ADCSRA, MASK(ADEN) | MASK(ADIE) | \
        MASK(ADPS2) | MASK(ADPS1) | MASK(ADPS0));
    //bit ADEN enable ADC
    //bit ADIE - ADC conversion complete interrupt enable
    //bits ADPS2 ADPS1 ADPS0 = 1 -> 128 prescaler (slowest conv time)

    ADCSRB = 0b0;
    SET(ADCSRB, MASK(ADTS1) | MASK(ADTS0));
    // ADC triggers when Timer 0 compare match A

    DIDR0 = 0b0;
    SET(DIDR0, MASK(ADC5D) | MASK(ADC4D) | MASK(ADC3D) | MASK(ADC2D) | MASK(ADC1D) | MASK(ADC0D));
    // disable digital input buffer on all ADC pins, since none of them are used digitally
}

// interrupt fires at 100Hz
ISR(TIMER0_COMPA_vect) {
    // if duty is positive, sample right OP AMP
    if (duty >= 0) {
        // to sample right, enable ADC1
        SET(ADMUX, MASK(MUX0));
    }
    else {
        // sample left, disable ADC1
        RESET(ADMUX, MASK(MUX0));
    }

    // start conversion
    ADCSRA |= (1 << ADSC);
}

// interrupt triggers when ADC conversion is complete
ISR(ADC_vect) {
    // fetch ADC value
    currentADCticks = ADC;

    // trigger controller execution
    runController = true;
}