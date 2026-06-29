#ifndef ATMEGA328P_INIT_H
#define ATMEGA328P_INIT_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void boardInit(void);
void setupWatchdog(void);
void setupPWMTimer(void);
void setupTimeCounter(void);
void enableInterrupts(void);

#endif // ATMEGA328P_INIT_H