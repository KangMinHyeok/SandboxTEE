#include "native_client/src/trusted/stdlib/string.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1



char* strcat(char* destination, const char* source) {
  char* ptr = destination + strlen(destination);
  while (*source != '\0') {
    *ptr++ = *source++;
  }
  *ptr = '\0';
  return destination;
}

#endif
