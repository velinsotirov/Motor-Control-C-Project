#define LB_HS 4
#define LB_LS 5
#define RB_HS 6
#define RB_LS 7
#define encoderA 2
#define encoderB 8
#define shuntMeasR A0
#define shuntMeasL A1

// bit operations
#define MASK(x) (0b1 << (x))
#define SET(p,m) ((p) |= (m))
#define RESET(p,m) ((p) &= ~(m))