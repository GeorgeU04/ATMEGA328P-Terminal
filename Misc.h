#ifndef MISC_H
#define MISC_H

#define __AVR_ATmega328P__
#include <stdint.h>

void stringToLower(char *string);
int32_t stringToInt(int32_t *num, char *string);

#endif // !MSIC_H
