#ifndef DIAG_H
#define DIAG_H

#include <stdint.h>
#include <stdbool.h>

#include "fixed_point.h"

typedef enum {
    COMMAND_EMPTY, // 0 is not a command since we might interpret empty UART frames as a command
    COMMAND_SPEED_MODE,
    COMMAND_TORQUE_MODE,
    COMMAND_SPEED_REF,
    COMMAND_TORQUE_REF,
    COMMAND_RUN,
    COMMAND_STOP
} diag_command_t;

void diag_step(void);

extern volatile bool diag_Execute;

bool returnDiagModeRequest(void);
bool returnDiagPowerStageRequest(void);
q4_12_t returnDiagTorqueRequest(void);
int16_t returnDiagSpeedRequest(void);

#endif // DIAG_H