#include <stdbool.h>

#include "fixed_point.h"

q8_8_t returnDiagPWMRequest(void);
uint8_t returnDiagModeRequest(void);
bool returnDiagPowerStageRequest(void);
q4_12_t returnDiagTorqueRequest(void);
int16_t returnDiagSpeedRequest(void);

extern uint8_t modeReq;