#include "native_client/src/trusted/stdlib/fileio.h"
#include "native_client/src/trusted/stdlib/filestruct.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1

int fflush(FILE *fp)
{
    int ret = ocall_fflush(fp);
    return ret;
}

#endif
