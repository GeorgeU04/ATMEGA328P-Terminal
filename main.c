#include "Commands.h"
#include "Misc.h"
#include "UART.h"
#include <avr/io.h>
#include <stdint.h>
#include <string.h>

#ifndef F_CPU
#define F_CPU 1000000UL
#endif /* ifndef F_CPU */

int32_t main(void) {
  uart_init(12);
  char input[65] = {0};
  uint8_t i = 0;
  DDRD |= (1 << DDD6) | (1 << DDD7);  // set for output
  PORTD &= ~(1 << PD6) | ~(1 << PD7); // set low

  struct command com = {0};
  while (1) {
    char dataRecieved = uart_receive();
    if (dataRecieved == ' ' && i > 0 && input[i - 1] == ' ') {
      PORTD |= (1 << PD6);
      continue;
    }

    if (dataRecieved == '\r') { // newline
      PORTD &= ~(1 << PD6);
      input[i] = '\0';
      parseCommand(input, &com);
      stringToLower(com.instruction);
      if (strcmp(com.instruction, "blink") == 0) {
        printCommand(&com);
        executeBlink(com.parameters);
      } else if (strcmp(com.instruction, "temp") == 0) {
        printCommand(&com);
        executeTemp(com.parameters);
      } else if (strcmp(com.instruction, "help") == 0) {
        uart_transmit_string("\n\rCommands:\n\r");
        uart_transmit_string("Blink\n\r -d <amount> \n\r");
        uart_transmit_string("Temp\n\r -f returns temp fahrenheit\n\r -c "
                             "returns temp celsius \n\r");
        uart_transmit_string("\n\rQuit: stops communication to device\n\r");
      } else if (strcmp(com.instruction, "quit") == 0) {
        uart_transmit_string("\n\rExiting Now\n\r");
        goto exit;
      } else {
        uart_transmit_string("\n\rUnrecognized Command\n\r");
      }
      input[0] = '\0';
      i = 0;

    } else if (dataRecieved == '\b' || dataRecieved == 127) { // backspace
      if (i > 0) {
        PORTD &= ~(1 << PD6);
        uart_transmit('\b');
        uart_transmit(' ');
        uart_transmit('\b');
        input[i--] = '\0';
      }

    } else { // normal input
      if (i < 63) {
        input[i++] = dataRecieved;
        uart_transmit(dataRecieved);
      } else { // buffer full
        PORTD |= (1 << PD6);
      }
    }
  }
exit:
  PORTD &= ~(1 << PD6);
  return 0;
}
