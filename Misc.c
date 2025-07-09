#include "Misc.h"
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void stringToLower(char *string) {
  while (*string != '\0') {
    *string = tolower(*string);
    string++;
  }
}

int32_t stringToInt(int32_t *num, char *string) {
  if (strcmp("NONE", string) == 0) {
    return -1;
  }
  for (size_t i = 0; i < strlen(string); ++i) {
    if (!isdigit(string[i])) {
      return -1;
    }
  }
  char *end;
  *num = strtol(string, &end, 10);
  if (end == string) {
    return -1;
  }
  if (*num > INT32_MAX || *num < INT32_MIN) {
    return -1;
  }
  return 0;
}
