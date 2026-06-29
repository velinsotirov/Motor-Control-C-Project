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
#define INT_TO_Q22_10(x) ((x) * Q22_10_SCALE)
#define Q22_10_TO_INT(x) ((x) / Q22_10_SCALE)

static inline q22_10_t q22_10_mul(q22_10_t x, q22_10_t y) {
    return (q22_10_t) ((((int32_t) (x)) * y) >> Q22_10_SHIFT);
}
static inline q22_10_t q22_10_div(q22_10_t x, q22_10_t y) {
    return (q22_10_t) ((((int32_t) (x)) << Q22_10_SHIFT) / y);
}

// fixed point type for 4.12 format
typedef int16_t q4_12_t;
#define Q4_12_SHIFT 12
#define Q4_12_SCALE (1 << Q4_12_SHIFT)

#define FLOAT_TO_Q4_12(x) ((q4_12_t) ((x) * Q4_12_SCALE))
#define Q4_12_TO_FLOAT(x) (((float) (x)) / Q4_12_SCALE)
#define INT_TO_Q4_12(x) ((x) * Q4_12_SCALE)
#define Q4_12_TO_INT(x) ((x) / Q4_12_SCALE)

static inline q4_12_t q4_12_mul(q4_12_t x, q4_12_t y) {
    return (q4_12_t) ((((int64_t) (x)) * y) >> Q4_12_SHIFT);
}
static inline q4_12_t q4_12_div(q4_12_t x, q4_12_t y) {
    return (q4_12_t) ((((int64_t) (x)) << Q4_12_SHIFT) / y);
}

// fixed point type for 8.8 format
typedef int16_t q8_8_t;
#define Q8_8_SHIFT 8
#define Q8_8_SCALE (1 << Q8_8_SHIFT)

#define FLOAT_TO_Q8_8(x) ((q8_8_t) ((x) * Q8_8_SCALE))
#define Q8_8_TO_FLOAT(x) (((float) (x)) / Q8_8_SCALE)
#define INT_TO_Q8_8(x) ((x) * Q8_8_SCALE)
#define Q8_8_TO_INT(x) ((x) / Q8_8_SCALE)

static inline q8_8_t q8_8_mul(q8_8_t x, q8_8_t y) {
    return (q8_8_t) ((((int64_t) (x)) * y) >> Q8_8_SHIFT);
}
static inline q8_8_t q8_8_div(q8_8_t x, q8_8_t y) {
    return (q8_8_t) ((((int64_t) (x)) << Q8_8_SHIFT) / y);
}

// fixed point type for 16.16 format
typedef int32_t q16_16_t;
#define Q16_16_SHIFT 16
#define Q16_16_SCALE (1 << Q16_16_SHIFT)

#define FLOAT_TO_Q16_16(x) ((q16_16_t) ((x) * Q16_16_SCALE))
#define Q16_16_TO_FLOAT(x) (((float) (x)) / Q16_16_SCALE)
#define INT_TO_Q16_16(x) ((x) * Q16_16_SCALE)
#define Q16_16_TO_INT(x) ((x) / Q16_16_SCALE)

static inline q16_16_t q16_16_mul(q16_16_t x, q16_16_t y) {
    return (q16_16_t) ((((int64_t) (x)) * y) >> Q16_16_SHIFT);
}
static inline q16_16_t q16_16_div(q16_16_t x, q16_16_t y) {
    return (q16_16_t) ((((int64_t) (x)) << Q16_16_SHIFT) / y);
}

// conversion
#define Q22_10_TO_Q8_8(x) ((q8_8_t) ((x) >> (Q22_10_SHIFT - Q8_8_SHIFT)))
#define Q4_12_TO_Q22_10(x) ((q22_10_t) (((q22_10_t)x) >> (Q4_12_SHIFT - Q22_10_SHIFT)))
#define Q4_12_TO_Q16_16(x) ((q16_16_t) ((q16_16_t)x) << (Q16_16_SHIFT - Q4_12_SHIFT))
#define Q16_16_TO_Q8_8(x) ((q8_8_t) ((x) >> (Q16_16_SHIFT - Q8_8_SHIFT)))

#endif // FIXED_POINT_H
