
#include "native_client/src/trusted/stdlib/fileio.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"

int fflush(FILE *fp)
{
    int ret = ocall_fflush(fp);
    return ret;
}
