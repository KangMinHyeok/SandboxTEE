#ifndef _FILEIO_H_
#define _FILEIO_H_
#include "native_client/src/trusted/stdlib/filestruct.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1
#include <bits/types.h>
int fflush(FILE *fp);
int fsync(int fd);
int ferror(FILE *fp);
long ftell(FILE *fp);
int fseek(FILE *fp, long offset, int whence);
FILE * fopen(const char *file, const char *mode);
int fclose(FILE *fp);
size_t fread(void *buf, size_t size, size_t count, register FILE *fp);
size_t fwrite(const void *buf, size_t size, size_t count, register FILE *fp);
int flock(int fd, int operation);
int fchmod(int fd, __mode_t mode);
//int fcntl(int fd, int cmd, long args);
extern int fcntl (int __fd, int __cmd, ...);
int fdatasync(int fd);
FILE *fdopen(int fd, const char *mode);
int ffs(register int mask);
int fileno(FILE *fp);
#endif

#endif

