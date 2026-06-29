#ifndef CURRENT_H
#define CURRENT_H

#include <stdint.h>

#include "fixed_point.h"

q4_12_t measureCurrent(void);

extern volatile uint16_t leftADCticks;
//extern volatile uint16_t rightADCticks;

#endif // CURRENT_H