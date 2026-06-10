
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "current.h"
#include "controller.h"
#include "atmega328p_adc.h"
#include "atmega328p_hal.h"

void setupADC() {
    ADMUX = 0b0;
    //pins REFS1, REFS0 = 0 -> we use AREF!
    //pins MUX3,2,1,0 0000 for ADC0

    ADCSRA = 0b0;
    SET(ADCSRA, MASK(ADEN) | MASK(ADIE) | \
        MASK(ADPS2) | MASK(ADPS0));
    //bit ADEN enable ADC
    //bit ADIE - ADC conversion complete interrupt enable
    //bits ADPS2, ADPS0 = 1 -> 32 prescaler
    ADCSRB = 0b0;

    DIDR0 = 0b0;
    SET(DIDR0, MASK(ADC5D) | MASK(ADC4D) | MASK(ADC3D) | MASK(ADC2D) | MASK(ADC1D) | MASK(ADC0D));
    // disable digital input buffer on all ADC pins, since none of them are used digitally
}

// interrupt fires at middle of PWM period
ISR(TIMER0_COMPB_vect) {
    // start conversion
    ADCSRA |= (1 << ADSC);
}

// interrupt triggers when ADC conversion is complete
ISR(ADC_vect) {
    // fetch ADC value
    currentADCticks = ADC;

    // if we wanted the motor control to trigger at 20kHz, we would set a flag here
}