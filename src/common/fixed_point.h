#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdio.h>
#include <stdint.h>

// fixed point type for 16.16 format, data range -32768 to 32767.9999, resolution 0.0000000152
typedef int32_t q16_16_t;

#define Q16_16_SHIFT 16
#define Q16_16_SCALE (1 << Q16_16_SHIFT)

#define FLOAT_TO_Q16_16(x) ((q16_16_t) ((x) * Q16_16_SCALE))
#define Q16_16_TO_FLOAT(x) (((float) (x)) / Q16_16_SCALE)

#define INT_TO_Q16_16(x) ((x) << Q16_16_SHIFT)
#define Q16_16_TO_INT(x) ((x) >> Q16_16_SHIFT)


static inline q16_16_t q16_16_mul(q16_16_t x, q16_16_t y) {
    return (q16_16_t) ((((int64_t) (x)) * y) >> Q16_16_SHIFT);
}

static inline q16_16_t q16_16_div(q16_16_t x, q16_16_t y) {
    return (q16_16_t) ((((int64_t) (x)) << Q16_16_SHIFT) / y);
}

#endif // FIXED_POINT_H
