
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "pinmap.h"

void uart_init() {
    UCSR0C = 0b0;
    // bits UMSELn1, UMSELn0 = 0 for asynchronous UART
    // bits UPMn1, UPMn0 = 0 for no parity
    // bit USBSn = 0 for 1 stop bit
    // bits UCSZn1 = 1, UCSZn1 = 1 -> 8 bit transmission
    // bit UCPOLn = 0 for falling edge
    SET(UCSR0C, (MASK(UCSZ01) | MASK(UCSZ00)));

    //fosc = 16MHz by default
    //U2Xn = 0 double speed mode disabled
    //UBRRn = 103 or 0b1100111 for 9600 and 16MHz, meaning UBRRnL = 0b1100111, UBRRnH = 0b0
    UBRR0L = 0b0; UBRR0H = 0b0;
    UBRR0L = 0b110011;

    //bit RXENn = 1 -> enable receive
    //bit TXENn = 1 -> enable trasmit
    UCSR0B = 0b0;
    SET(UCSR0B, MASK(RXEN0) | MASK(TXEN0));
}

//UCSR0A
//bit RXCn = 1 -> unread data in receive buffer
//bit UDREn = 1 -> transmit buffer empty (we can send)