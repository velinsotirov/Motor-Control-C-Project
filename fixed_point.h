#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdio.h>

typedef short int q8_8_t;
typedef short int q10_6_t;

#define Q8_8_SHIFT 8
#define Q8_8_SCALE (1 << Q8_8_SHIFT)

#define Q10_6_SHIFT 6
#define Q10_6_SCALE (1 << Q10_6_SHIFT)

#define FLOAT_TO_Q8_8(x) ((q8_8_t) ((x) * Q8_8_SCALE))
#define Q8_8_TO_FLOAT(x) (((float) (x)) / Q8_8_SCALE)

#define FLOAT_TO_Q10_6(x) ((q10_6_t) ((x) * Q10_6_SCALE))
#define Q10_6_TO_FLOAT(x) (((float) (x)) / Q10_6_SCALE)

#define Q8_8_TO_Q10_6(x) ((q10_6_t) ((x) >> 2))
#define Q10_6_TO_Q8_8(x) ((q8_8_t) ((x) << 2))

static inline q8_8_t q8_8_mul(q8_8_t x, q8_8_t y) {
    return (q8_8_t) ((((int) (x)) * y) >> Q8_8_SHIFT);
}

static inline q8_8_t q8_8_div(q8_8_t x, q8_8_t y) {
    return (q8_8_t) ((((int) (x)) << Q8_8_SHIFT) / y);
}

static inline q10_6_t q10_6_mul(q10_6_t x, q10_6_t y) {
    return (q10_6_t) ((((int) (x)) * y) >> Q10_6_SHIFT);
}

static inline q10_6_t q10_6_div(q10_6_t x, q10_6_t y) {
    return (q10_6_t) ((((int) (x)) << Q10_6_SHIFT) / y);
}

#endif // FIXED_POINT_H
