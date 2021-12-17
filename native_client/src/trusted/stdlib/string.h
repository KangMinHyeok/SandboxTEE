#ifndef _STRING_H_
#define _STRING_H_

#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/stdlib/stdbool.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#if NACL_SGX == 1
// strcpy.c
char *strcpy(char *dst, const char* src);
char *strncpy(char *dst, const char *src, size_t n);

// strlen.c
size_t strnlen (const char *str, size_t maxlen);
size_t strlen (const char *str);

// strchr.c
bool isspace(int c);
bool isdigit(int c);
bool isalpha(int c);
bool isupper(int c);
char *strchr(const char *s, int c_in);
char *strpbrk(const char *s1, const char *s2);
unsigned long strtoul(const char *nptr, char **endptr, register int base);
// TODO(mhkang): check if it is in use.
//extern char *sgx_strdup(const char *s);

// strcmp.c
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);

// strdup.c
extern char * strdup(const char *s);
extern char * strndup(const char *s, size_t n);

// strtol.c
extern long strtol(const char *nptr, char **endptr, register int base);

// strcat.c
extern char *strcat(char *destination, const char *source); 


#endif

#endif
