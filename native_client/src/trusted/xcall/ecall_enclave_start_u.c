#include "native_client/src/trusted/xcall/ecall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"


int ecall_enclave_start (void)
{

	ms_ecall_enclave_start_t ms;
	ms.temp = 0;

	return sgx_ecall(ECALL_ENCLAVE_START, &ms);
}

