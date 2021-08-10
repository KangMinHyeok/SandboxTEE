#include "native_client/src/trusted/stdlib/fileio.h"
#include "native_client/src/trusted/stdlib/filestruct.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1

int fflush(FILE *fp)
{
    int ret = ocall_fflush(fp);
    return ret;
}

int fsync(int fd)
{
    int ret = ocall_fsync(fd);
    return ret;
}

int ferror(FILE *fp)
{
    int ret = ocall_ferror(fp);
    return ret;
}

long ftell(FILE *fp)
{
    long ret = ocall_ftell(fp);
    return ret;
}

int fseek(FILE *fp, long offset, int whence)
{
    int ret = ocall_fseek(fp, offset, whence);
    return ret;
}

FILE * fopen(const char *file, const char *mode)
{
    FILE * fp = ocall_fopen(file, mode);
    return fp;
}

int fclose(FILE *fp)
{
    int ret = ocall_fclose(fp);
    return ret;
}

size_t fread(void * buf, size_t size, size_t count, register FILE *fp)
{
    size_t ret = ocall_fread(buf, size, count, fp);
    return ret;
}

size_t fwrite(const void *buf, size_t size, size_t count, register FILE *fp)
{
    size_t ret = ocall_fwrite(buf, size, count, fp);
    return ret;
}

#endif
