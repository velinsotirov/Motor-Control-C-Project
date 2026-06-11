
#include <stdbool.h>
#include <stdint.h>

#include "diag.h"
#include "global.h"
#include "system.h"
#include "current.h"
#include "encoder.h"
#include "controller.h"
#include "ringbuffer.h"

#ifdef __AVR_ATmega328P__
#include "atmega328p_uart.h"
//16MHz / (256 * (124 + 1)) -> 500Hz, we want 10Hz rx so flag is 50
// we want 2Hz Tx so flag is 250
#define RX_COUNT 50u
#define TX_COUNT 250u
#else
#include "stm32_uart.h"
// systick is 1000Hz, so counters are twice as large
#define RX_COUNT 100u
#define TX_COUNT 500u
#endif

const uint16_t t_step_tx = TX_COUNT; // when counter has moved this much, its time to execute tx
const uint16_t t_step_rx = RX_COUNT; // when counter has moved this much, its time to execute rx
uint16_t intermediary_ref_value = 0;

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
    // controller mode and state
    uint8_t controller_state = (uint8_t) get_controller_state();
    bool controller_mode = (bool) get_controller_mode();
    // put into one uint_t frame
    uint8_t combined_state_var = controller_state | (controller_mode << 7);

    // fetch remaining data
    q4_12_t current = measureCurrent();
    q4_12_t torque = q4_12_mul(current, K_times_Psi_q4_12);
    int16_t speed = get_motor_speed_est();
    int8_t duty = get_motor_duty();

    // push all but 1st tx packets into tx rb
    ringbuffer_write(&tx_rb, (current >> 8));
    ringbuffer_write(&tx_rb, (current & 255u));
    ringbuffer_write(&tx_rb, (torque >> 8));
    ringbuffer_write(&tx_rb, (torque & 255u));
    ringbuffer_write(&tx_rb, (speed >> 8));
    ringbuffer_write(&tx_rb, (speed & 255u));
    ringbuffer_write(&tx_rb, duty);

    // load first byte into register and enable tx interrupt
    writeToUSART(&combined_state_var);
    enableTxInterrupt();
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
