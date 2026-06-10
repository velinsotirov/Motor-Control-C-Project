#ifndef ATMEGA328P_INIT_H
#define ATMEGA328P_INIT_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void boardInit(void);
void setPWMTimer(void);
void setupTimeCounter(void);

#endif // ATMEGA328P_INIT_H