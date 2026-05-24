
#include <stdint.h>

#include "fixed_point.h"

typedef enum {
    COMMAND_SPEED_MODE,
    COMMAND_TORQUE_MODE,
    COMMAND_SPEED_REF,
    COMMAND_TORQUE_REF,
    COMMAND_RUN,
    COMMAND_STOP
} diag_command_t;

void diag_step(void);

bool returnDiagModeRequest(void);
bool returnDiagPowerStageRequest(void);
q4_12_t returnDiagTorqueRequest(void);
int16_t returnDiagSpeedRequest(void);
