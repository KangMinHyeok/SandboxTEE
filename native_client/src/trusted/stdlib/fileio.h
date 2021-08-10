#ifndef _FILEIO_H_
#define _FILEIO_H_
#include "native_client/src/trusted/stdlib/filestruct.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1

int fflush(FILE *fp);
int fsync(int fd);
int ferror(FILE *fp);
long ftell(FILE *fp);
int fseek(FILE *fp, long offset, int whence);
FILE * fopen(const char *file, const char *mode);
int fclose(FILE *fp);
size_t fread(void *buf, size_t size, size_t count, register FILE *fp);
size_t fwrite(const void *buf, size_t size, size_t count, register FILE *fp);
#endif

#endif

