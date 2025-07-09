#ifndef UART_H
#define UART_H

#include <stdint.h>
void uart_init(uint8_t ubrr);
char uart_receive(void);
void uart_transmit(char data);
void uart_transmit_string(char *str);
#endif // !UART_H
