#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdio.h>
#include <stdint.h>

// fixed point type for 22.10 format
typedef int32_t q22_10_t;

#define Q22_10_SHIFT 10
#define Q22_10_SCALE (1 << Q22_10_SHIFT)

#define FLOAT_TO_Q22_10(x) ((q22_10_t) ((x) * Q22_10_SCALE))
#define Q22_10_TO_FLOAT(x) (((float) (x)) / Q22_10_SCALE)

#define INT_TO_Q22_10(x) ((x) << Q22_10_SHIFT)
#define Q22_10_TO_INT(x) ((x) >> Q22_10_SHIFT)


static inline q22_10_t q22_10_mul(q22_10_t x, q22_10_t y) {
    return (q22_10_t) ((((int32_t) (x)) * y) >> Q22_10_SHIFT);
}

static inline q22_10_t q22_10_div(q22_10_t x, q22_10_t y) {
    return (q22_10_t) ((((int32_t) (x)) << Q22_10_SHIFT) / y);
}

#endif // FIXED_POINT_H
