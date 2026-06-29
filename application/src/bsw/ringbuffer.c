#include <stdint.h>
#include <stdbool.h>

#include "ringbuffer.h"

#ifdef __AVR_ATmega328P__
#include "atmega328p_hal.h"
#elif defined(__ARM_CortexM3__)
#include "stm32_hal.h"
#endif

// two ring buffers
uint8_t rx_init[RX_RB_LEN];
uint8_t tx_init[TX_RB_LEN];
rb_type_t rx_rb = {RX_RB_LEN,rx_init,0u,0u,RX_MOD_MASK};
rb_type_t tx_rb = {TX_RB_LEN,tx_init,0u,0u,TX_MOD_MASK};

void ringbuffer_write(rb_type_t *rb_struct, uint8_t data_in) {
    // write data to write pointer
    rb_struct->rb[rb_struct->wp] = data_in;

    // advance read pointer and wrap
    rb_struct->wp += 1;
    rb_struct->wp &= rb_struct->mask;
}

bool ringbuffer_read(rb_type_t *rb_struct, uint8_t* received_msg, uint8_t message_len) {
    // disable interrupts for safety and fetch local copies of pointers
    uint32_t sreg = disableInterrupts();

    // number of new bytes
    uint8_t bytes_available = (rb_struct->wp - rb_struct->rp) & rb_struct->mask;
    // if less than length of entire package, return and reenable interrupts immediately
    if (bytes_available < message_len) {
        RESTORE_SREG(sreg);
        return false;
    }

    // we have new data, so read it
    for (int i = 0; i < message_len; i++) {
        // read byte
        received_msg[i] = rb_struct->rb[rb_struct->rp];

        // advance read pointer and wrap
        rb_struct->rp += 1;
        rb_struct->rp &= rb_struct->mask;
    }

    // enable interrupts and return true
    RESTORE_SREG(sreg);
    return true;
}
