#ifndef DIAG_H
#define DIAG_H

#include <stdint.h>
#include <stdbool.h>

#include "fixed_point.h"

#define RX_PACKET_LEN 3u
#define TX_PACKET_LEN 8u

extern uint8_t received_msg[RX_PACKET_LEN];
extern uint8_t outgoing_msg[TX_PACKET_LEN];

extern const uint16_t t_step_rx;
extern const uint16_t t_step_tx;

typedef enum {
    COMMAND_EMPTY = 0u, // 0 is not a command since we might interpret empty UART frames as a command
    COMMAND_RUN,
    COMMAND_STOP,
    COMMAND_SPEED_MODE,
    COMMAND_TORQUE_MODE,
    COMMAND_SPEED_REF,
    COMMAND_TORQUE_REF
} diag_command_t;

void diag_step_100ms(void);
void diag_step_500ms(void);

bool returnDiagModeRequest(void);
bool returnDiagPowerStageRequest(void);
q4_12_t returnDiagTorqueRequest(void);
int16_t returnDiagSpeedRequest(void);

#endif // DIAG_H