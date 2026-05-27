
#include <stdbool.h>
#include <stdint.h>

#include "diag.h"
#include "system.h"
#include "ringbuffer.h"
#include "atmega328p_uart.h"

volatile bool diag_tx_send = false;
uint16_t intermediary_ref_value = 0;

// time step of rx read
const uint16_t t_step_rx = 6249;

bool diag_speed_mode_req = false;
bool diag_powerstage_req = false;
q4_12_t torque_diag_ref = 0;
uint16_t speed_diag_ref = 0;

uint8_t received_msg[RX_PACKET_LEN] = {};
uint8_t outgoing_msg[TX_PACKET_LEN] = {};

void diag_step_100ms() {
    // check UART for imcoming commands
    // if at least 3 bytes present in buffer, read 3 bytes, put them in message_array and return true
    if (ringbuffer_read(&rx_rb, received_msg, RX_PACKET_LEN)) {

        // check type of command and decode message into 
        switch (received_msg[0]) {
            case COMMAND_TORQUE_MODE:
                // switch mode only, the next two bytes are ignored
                diag_speed_mode_req = false;
                break;
            case COMMAND_SPEED_MODE:
                // switch mode only, the next two bytes are ignored
                diag_speed_mode_req = true;
                break;
            case COMMAND_TORQUE_REF:
                // fetch two bytes into intermediary value
                intermediary_ref_value = (received_msg[1] << 8 | received_msg[2]);

                // convert and set request
                torque_diag_ref = INT_TO_Q4_12(intermediary_ref_value);
                break;
            case COMMAND_SPEED_REF:
                // fetch two bytes into intermediary value
                intermediary_ref_value = (received_msg[1] << 8 | received_msg[2]);

                // convert and set request
                speed_diag_ref = intermediary_ref_value;
                break;
            case COMMAND_RUN:
                diag_powerstage_req = true;
                break;
            case COMMAND_STOP:
                diag_powerstage_req = false;
                break;
        }
    }
}

void diag_step_1000ms() {
    // dummy data for now
    uint8_t dummy_byte = 127u;

    // push tx packet into tx rb
    for (uint8_t i=1; i < (uint8_t) TX_PACKET_LEN; i++) {
        ringbuffer_write(&tx_rb, dummy_byte);
    }
    // load first byte into register and enable tx interrupt
    writeToUSART(&dummy_byte);
    enableTxInterrupt();
}

// executes every 1s and triggers diag_Send in main loop
void diagTrigger() {
  diag_tx_send = true;
}

bool returnDiagModeRequest(void) {
    return diag_speed_mode_req;
}
bool returnDiagPowerStageRequest(void) {
    return diag_powerstage_req;
}
q4_12_t returnDiagTorqueRequest(void) {
    return torque_diag_ref;
}
int16_t returnDiagSpeedRequest(void) {
    return speed_diag_ref;
}
