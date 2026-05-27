#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define RX_RB_LEN 8u
#define TX_RB_LEN 16u
#define RX_MOD_MASK (RX_RB_LEN - 1u)
#define TX_MOD_MASK (TX_RB_LEN - 1u)

typedef struct {
    uint8_t len;
    uint8_t *rb;
    volatile uint8_t rp;
    volatile uint8_t wp;
    uint8_t mask;
} rb_type_t;

extern rb_type_t rx_rb;
extern rb_type_t tx_rb;

void ringbuffer_write(rb_type_t*, uint8_t);
bool ringbuffer_read(rb_type_t*, uint8_t*, uint8_t);

#endif // RINGBUFFER_H