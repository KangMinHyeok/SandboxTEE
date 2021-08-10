#ifndef _FILEIO_H_
#define _FILEIO_H_
#include "native_client/src/trusted/stdlib/filestruct.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1

int fflush(FILE *fp);
#endif

#endif

