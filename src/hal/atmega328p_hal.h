#ifndef ATMEGA328P_HAL_H
#define ATMEGA328P_HAL_H

#include <avr/io.h>
#include <avr/interrupt.h>

// define ports B,C,D as 2 bits which are shifted left so they dont
// collide with the numbering of the pin within its port (3 bits)
#define PORT_SHIFT 3
#define PIN_MASK 0b111
#define portB (0u << PORT_SHIFT)
#define portC (1u << PORT_SHIFT)
#define portD (2u << PORT_SHIFT)

#define VALID_PINS_B 0xFF // PB0 - PB7 exist
#define VALID_PINS_C 0x3F // PC0 - PC5 exist (PC6 is RESET)
#define VALID_PINS_D 0xFF // PD0 - PD7 exist

// generic macros
#define MASK(x) (0b1 << (x))
#define SET(p,m) ((p) |= (m))
#define RESET(p,m) ((p) &= ~(m))
#define TOGGLE(p,m) ((p) ^= (m))
#define ISHIGH(p,m) ((p) & (m))

// helper macros
#define GET_PORT_NUM(pin) ((pin) >> PORT_SHIFT)
#define GET_PIN_LOCAL_NUM(pin) ((pin) & PIN_MASK)
#define GET_PORT_FROM_PIN(pin) (GET_PORT_NUM(pin)==0 ? PORTB : GET_PORT_NUM(pin)==1 ? PORTC : PORTD)
#define GET_READPORT_FROM_PIN(pin) (GET_PORT_NUM(pin)==0 ? PINB : GET_PORT_NUM(pin)==1 ? PINC : PIND)
#define PIN_LOCAL_MASK(pin) (MASK(GET_PIN_LOCAL_NUM(pin)))

// HAL macros to be used by ASW
#define HAL_SET_PIN(pin) \
    (GET_PORT_NUM(pin) == 0 ? (PORTB |= PIN_LOCAL_MASK(pin)) : \
     GET_PORT_NUM(pin) == 1 ? (PORTC |= PIN_LOCAL_MASK(pin)) : \
                              (PORTD |= PIN_LOCAL_MASK(pin)))

#define HAL_RESET_PIN(pin) \
    (GET_PORT_NUM(pin) == 0 ? (PORTB &= ~PIN_LOCAL_MASK(pin)) : \
     GET_PORT_NUM(pin) == 1 ? (PORTC &= ~PIN_LOCAL_MASK(pin)) : \
                              (PORTD &= ~PIN_LOCAL_MASK(pin)))

#define HAL_TOGGLE_PIN(pin) \
    (GET_PORT_NUM(pin) == 0 ? (PINB |= PIN_LOCAL_MASK(pin)) : \
     GET_PORT_NUM(pin) == 1 ? (PINC |= PIN_LOCAL_MASK(pin)) : \
                              (PIND |= PIN_LOCAL_MASK(pin)))

#define HAL_IS_PIN_HIGH(pin) ( ISHIGH(GET_READPORT_FROM_PIN(pin), MASK(GET_PIN_LOCAL_NUM(pin))) )

// HAL macro to be used by board init
#define GET_DDREG_FROM_PIN(pin) ( GET_PORT_NUM(pin)==0 ? DDRB : GET_PORT_NUM(pin)==1 ? DDRC : DDRD )

#define SET_PIN_AS_INPUT(pin) \
    (GET_PORT_NUM(pin) == 0 ? (DDRB &= ~PIN_LOCAL_MASK(pin)) : \
     GET_PORT_NUM(pin) == 1 ? (DDRC &= ~PIN_LOCAL_MASK(pin)) : \
                              (DDRD &= ~PIN_LOCAL_MASK(pin)))

#define SET_PIN_AS_OUTPUT(pin) \
    (GET_PORT_NUM(pin) == 0 ? (DDRB |= PIN_LOCAL_MASK(pin)) : \
     GET_PORT_NUM(pin) == 1 ? (DDRC |= PIN_LOCAL_MASK(pin)) : \
                              (DDRD |= PIN_LOCAL_MASK(pin)))

// mapping MCU pin numbers to port name and local pin number within port
// DO NOT CHANGE!!! THIS IS FIXED FOR THIS BOARD!!!
typedef enum {
    IO_1 = portD | 3u,
    IO_2 = portD | 4u,
    IO_7 = portB | 6u,
    IO_8 = portB | 7u,
    IO_9 = portD | 5u,
    IO_10 = portD | 6u,
    IO_11 = portD | 7u,
    IO_12 = portB | 0u,
    IO_13 = portB | 1u,
    IO_14 = portB | 2u,
    IO_15 = portB | 3u,
    IO_16 = portB | 4u,
    IO_17 = portB | 5u,
    IO_23 = portC | 0u,
    IO_24 = portC | 1u,
    IO_25 = portC | 2u,
    IO_26 = portC | 3u,
    IO_27 = portC | 4u,
    IO_28 = portC | 5u,
    IO_29 = portC | 6u,
    IO_30 = portD | 0u,
    IO_31 = portD | 1u,
    IO_32 = portD | 2u
} board_pinmap_en;

// mapping PCB pin names to MCU pin numbers
// change this depending on actual MCU to PCB connections
typedef enum {
    PIN_ENCODER_A = IO_32,
    PIN_ENCODER_B = IO_12,
    PIN_LEFT_HS = IO_2,
    PIN_LEFT_LS = IO_9,
    PIN_RIGHT_HS = IO_10,
    PIN_RIGHT_LS = IO_11,
    PIN_SHUNT_L = IO_23,
    PIN_SHUNT_R = IO_24
} active_pins_en;

// bit mask of which pins are currently used
// change this depending on the above enum
#define USED_PINS_PORTB (PIN_LOCAL_MASK(PIN_ENCODER_B))
#define USED_PINS_PORTC (PIN_LOCAL_MASK(PIN_SHUNT_L) | PIN_LOCAL_MASK(PIN_SHUNT_R))
#define USED_PINS_PORTD (PIN_LOCAL_MASK(PIN_ENCODER_A) | PIN_LOCAL_MASK(PIN_LEFT_HS) \
                        | PIN_LOCAL_MASK(PIN_LEFT_LS) | PIN_LOCAL_MASK(PIN_RIGHT_HS) \
                        | PIN_LOCAL_MASK(PIN_RIGHT_LS))

uint8_t disableInterrupts(void);

#define TIMER_VAL TCNT1
#define RESTORE_SREG(sreg) SREG = sreg

#endif // ATMEGA328P_HAL_H