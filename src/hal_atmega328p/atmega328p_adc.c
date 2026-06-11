
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "current.h"
#include "atmega328p_adc.h"
#include "atmega328p_hal.h"

void setupADC() {
    ADMUX = 0b0;
    SET(ADMUX, MASK(REFS0));
    //pins REFS0 = 1 -> we use Vcc!
    //pins MUX3,2,1,0 0000 for ADC0

    ADCSRA = 0b0;
    SET(ADCSRA, MASK(ADEN) | MASK(ADIE) | MASK(ADPS2) | MASK(ADPS0));
    //bit ADEN enable ADC
    //bit ADIE - ADC conversion complete interrupt enable
    //bits ADPS2, ADPS0 = 1 -> 32 prescaler
    ADCSRB = 0b0;

    DIDR0 = 0b0;
    SET(DIDR0, MASK(ADC0D)); // disable digital input buffer on ADC0 pin

    // first adc measurement to calibrate
    SET(ADCSRA, MASK(ADSC));
    while (ADCSRA & (1 << ADSC)); // discard first result
}

// interrupt fires at middle of PWM period
ISR(TIMER0_COMPB_vect) {
    // start conversion
    SET(ADCSRA, MASK(ADSC));
}

// interrupt triggers when ADC conversion is complete
ISR(ADC_vect) {
    // fetch ADC value
    currentADCticks = ADC;

    // if we wanted the motor control to trigger at 20kHz, we would set a flag here
}