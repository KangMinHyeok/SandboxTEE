
#include "native_client/src/trusted/xcall/ocall_types.h"

typedef int (*sgx_ocall_fn_t)(void*);

sgx_ocall_fn_t ocall_table[OCALL_NR] = {
	[OCALL_GETTIMEOFDAY] =  sgx_ocall_gettimeofday,
	[OCALL_DEBUGP] =  sgx_ocall_debugp,
};


