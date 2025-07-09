#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

struct command {
  char instruction[33];
  char parameters[33];
};

void parseCommand(char *command, struct command *com);
void executeBlink(char *parameter);
void executeTemp(char *parameter);
void printCommand(struct command *com);

int16_t readTemperature(void);
void startConversion(void);
#endif // !COMMANDS_H
