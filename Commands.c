#include <limits.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#include "Commands.h"
#include "Misc.h"
#include "UART.h"
#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

// parse command and get parameters
void parseCommand(char *input, struct command *com) {
  uint8_t i = 0;
  uint8_t j = 0;
  for (; input[i] != ' ' || i == 32; ++i) {
    if (input[i] == '\0') {
      strncpy(com->parameters, "NONE", 32);
      com->instruction[i] = '\0';
      return;
    }
    com->instruction[i] = input[i];
  }
  com->instruction[i] = '\0';
  ++i;
  if (input[i] == '\0') {
    strncpy(com->parameters, "NONE", 32);
    return;
  }
  for (; input[i] != '\0' || j == 32; ++j, ++i) {
    com->parameters[j] = input[i];
  }
  com->parameters[j] = '\0';
}

// blink LED
void executeBlink(char *parameter) {
  int32_t count = 0;
  if (stringToInt(&count, parameter) == -1) {
    uart_transmit_string("ERROR: Parameter Must be Integer Value.\n\r");
    return;
  }
  uart_transmit_string("Executing Blink...\n\r");
  for (int32_t i = 0; i < count; ++i) {
    PORTD |= (1 << PD7);
    _delay_ms(1000);
    PORTD &= ~(1 << PD7);
    _delay_ms(1000);
  }
  uart_transmit_string("Blink complete.\n\r");
}

void writeBit(uint8_t bit) {
  DDRD |= (1 << PD5); // set output

  PORTD &= ~(1 << PD5); // set low

  if (bit == 0) {
    _delay_us(60);
    PORTD |= (1 << PD5); // set high
  } else {
    _delay_us(10);
    PORTD |= (1 << PD5); // set high
    _delay_us(50);
  }
}

void writeByte(uint8_t byte) {
  for (uint8_t i = 0; i < 8; ++i) {
    writeBit(byte & 0x01);
    byte >>= 1;
  }
}

uint8_t readBit(void) {
  uint8_t bit = 0;
  DDRD |= (1 << PD5);   // set output
  PORTD &= ~(1 << PD5); // set low
  _delay_us(5);
  DDRD &= ~(1 << PD5); // set input
  _delay_us(15);
  bit = (PIND & (1 << PD5)) ? 1 : 0;
  _delay_us(45);

  return bit;
}

uint8_t readByte(void) {
  uint8_t byte = 0;

  for (uint8_t i = 0; i < 8; ++i) {
    byte |= (readBit() << i);
  }
  return byte;
}

uint8_t sendReset(void) {
  uint8_t response = 0;
  DDRD |= (1 << PD5);   // set output
  PORTD &= ~(1 << PD5); // set low
  _delay_us(480);
  DDRD &= ~(1 << PD5); // set input
  _delay_us(60);
  response = !(PIND & (1 << PD5));
  _delay_us(420);

  return response;
}

void startConversion(void) {
  sendReset();     // Reset the bus
  writeByte(0xCC); // Skip ROM command (if only one DS18B20)
  writeByte(0x44); // Start temperature conversion
}

int16_t readTemperature(void) {
  sendReset();     // Reset the bus
  writeByte(0xCC); // Skip ROM command (if only one DS18B20)
  writeByte(0xBE); // Read Scratchpad command

  uint8_t lsb = readByte(); // Read LSB
  uint8_t msb = readByte(); // Read MSB

  // Combine LSB and MSB to get raw temperature (12-bit resolution)
  int16_t raw_temp = (msb << 8) | lsb;

  return raw_temp / 16;
}

void executeTemp(char *parameter) {
  char transmit[256];
  startConversion();
  _delay_ms(750);
  int16_t tempC = readTemperature();
  int16_t currentTemp = tempC;
  if (strcmp("-f", parameter) == 0) {
    currentTemp = currentTemp * 1.8 + 32;
    snprintf(transmit, 256, "Temp F: %" PRId16 "\n\r", currentTemp);
  } else {
    snprintf(transmit, 256, "Temp C: %" PRId16 "\n\r", currentTemp);
  }
  uart_transmit_string(transmit);
}

void printCommand(struct command *com) {
  uart_transmit_string("\n\r");
  uart_transmit_string("Instruction: ");
  uart_transmit_string(com->instruction);
  uart_transmit_string("\n\r");
  uart_transmit_string("Parameters: ");
  uart_transmit_string(com->parameters);
  uart_transmit_string("\n\r");
}
