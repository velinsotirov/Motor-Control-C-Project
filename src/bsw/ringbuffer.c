#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define MODULO_MASK 0b111

// ring buffer is static to this translational unit
static volatile uint8_t ringbuffer[8] = {};

// accessable to interrupts
volatile uint8_t write_pointer = 0;
volatile uint8_t read_pointer = 0;

// used in main code
const uint8_t message_len = 3;

void ringbuffer_write(uint8_t data_in) {
    // write data to write pointer
    ringbuffer[write_pointer] = data_in;

    // advance read pointer and wrap to 8
    write_pointer += 1;
    write_pointer %= MODULO_MASK;
}

bool ringbuffer_read(uint8_t* received_msg) {
    // disable interrupts for safety and fetch local copies of pointers
    cli(); 

    // number of new bytes
    uint8_t bytes_available = (write_pointer - read_pointer) & MODULO_MASK;
    // if less than 3, return and reenable interrupts immediately
    if (bytes_available < message_len) {
        sei();
        return false;
    }

    // we have new data, so read it
    for (int i=0; i<message_len; i++) {
        // read byte
        received_msg[i] = ringbuffer[read_pointer];

        // reset byte we just read to 0
        ringbuffer[read_pointer] = 0u;

        // advance read pointer
        read_pointer += 1;
        read_pointer %= MODULO_MASK;
    }

    // enable interrupts and return true
    sei();
    return true;
}
