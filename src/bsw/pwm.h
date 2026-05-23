
// duty which is read by PWM interrupt
volatile int duty_compa = 0;

// pwm counter
volatile int pwmCounter = 0;

// duty commands to all 4 MOSFETs (LH, duty high and low, RH, duty high and low)
volatile char FET_cmd = 0b0000;

void pwmCommands(void);

void pwmCount(void);

void setPWMTimerInterrupt(void);