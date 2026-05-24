#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>

extern volatile uint8_t write_pointer;
extern volatile uint8_t read_pointer;
extern const uint8_t message_len;

void ringbuffer_write(uint8_t);
bool ringbuffer_read(uint8_t*);

#endif // RINGBUFFER_H