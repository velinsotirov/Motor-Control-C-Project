#ifndef CURRENT_H
#define CURRENT_H
//#endif // CURRENT_H
#include <stdint.h>

#include "fixed_point.h"

q4_12_t measureCurrent(void);

void setupADC(void);
void setupADCCounterInterrupt(void);

extern volatile uint16_t currentADCticks; 

#endif // CURRENT_H