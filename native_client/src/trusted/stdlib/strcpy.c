#include "native_client/src/trusted/stdlib/string.h"
#include "native_client/src/include/build_config.h"
#if NACL_SGX == 1
char * strcpy(char *dst, const char* src){
    char *start = dst;
    for (; (*dst = *src) != '\0'; ++src, ++dst);
    return start;
}

char * strncpy(char *dst, const char *src, size_t n)
{
  if (n != 0) {
    char *d = dst;
    const char *s = src;
    do { 
      if ((*d++ = *s++) == 0) {
        /* NUL pad the remaining n-1 bytes */
        while (--n != 0)
          *d++ = 0;
        break;
      }
    } while (--n != 0);
  }
  return (dst);
}

#endif
