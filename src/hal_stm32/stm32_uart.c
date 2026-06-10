
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "diag.h"
#include "ringbuffer.h"
#include "stm32_hal.h"
#include "stm32_uart.h"

void writeToUSART(uint8_t* data_out) { // TODO: REWRITE!
  UDR0 = *data_out;
}

// enable and disable UART interrupts, abstracted for use by ASW
void enableTxInterrupt() { // TODO: REWRITE!
  SET(UCSR0B, MASK(UDRIE0));
}
void disableTxInterrupt() { // TODO: REWRITE!
  RESET(UCSR0B, MASK(UDRIE0));
}

void uart_init() { // TODO: REWRITE!
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
    //bit RXCIE0 = 1 -> enable interrupt when byte is received
    //bit UDRIE0 = 1 -> enable interrupt when uart byte has been sent and we can send again
    UCSR0B = 0b0;
    SET(UCSR0B, MASK(RXEN0) | MASK(TXEN0) | MASK(RXCIE0) | MASK(UDRIE0));
}

// interrupt when uart byte has been received and push it to ring buffer
ISR(USART_RX_vect) // TODO: REWRITE!
{
    ringbuffer_write(&rx_rb, UDR0);
}

// interrupt when uart buffer is empty so we can send our byte
// this interrupt is enabled when we have a tx packet ready
// and immediately disables itself once the entire packet has been sent out
ISR(USART_UDRE_vect) // TODO: REWRITE!
{
    // check if ring buffer has the data
    uint8_t oldest_byte = 0u;
    if (ringbuffer_read(&tx_rb, &oldest_byte, 1u)) {
        // we removed the byte from the rb and we send it out
        writeToUSART(&oldest_byte);
    }
    // tx buffer is empty, we've sent all bytes so we can disable the interrupt
    // it is re-enabled once we have another tx packet ready
    else {
        disableTxInterrupt();
    }
}
