
#include "fixed_point.h"

bool returnDiagModeRequest(void);
bool returnDiagPowerStageRequest(void);
q4_12_t returnDiagTorqueRequest(void);
int16_t returnDiagSpeedRequest(void);

extern bool speedModeReq;