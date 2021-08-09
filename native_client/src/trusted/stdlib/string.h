#ifndef _STRING_H_
#define _STRING_H_

#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/stdlib/stdbool.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#if NACL_SGX == 1
// strcpy.c
extern char *strcpy(char *dst, const char* src);
extern char *strncpy(char *dst, const char *src, size_t n);

// strlen.c
extern size_t strnlen (const char *str, size_t maxlen);
extern size_t strlen (const char *str);

// strchr.c
extern bool isspace(int c);
extern bool isdigit(int c);
extern bool isalpha(int c);
extern bool isupper(int c);
extern char *strchr(const char *s, int c_in);
extern char *strpbrk(const char *s1, const char *s2);
extern int strcmp(const char *s1, const char *s2);
extern unsigned long strtoul(const char *nptr, char **endptr, register int base);
// TODO(mhkang): check if it is in use.
//extern char *sgx_strdup(const char *s);

#endif

#endif
