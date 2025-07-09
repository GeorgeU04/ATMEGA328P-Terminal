#include "UART.h"
#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>

void uart_init(uint8_t ubrr) {
  // Set baud rate
  UBRR0H = (uint8_t)(ubrr >> 8);
  UBRR0L = (uint8_t)ubrr;

  // Enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // Set frame format: 8 data bits, 1 stop bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Put data into buffer, sends the data
  UDR0 = data;
}

char uart_receive(void) {
  // Wait for data to be received
  while (!(UCSR0A & (1 << RXC0)))
    ;

  // Get and return received data from buffer
  return UDR0;
}

void uart_transmit_string(char *str) {
  for (size_t i = 0; str[i] != '\0'; ++i) {
    uart_transmit(str[i]);
  }
}
