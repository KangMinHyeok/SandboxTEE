#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/ecall_types.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_tls.h"

#include "native_client/src/trusted/xcall/enclave_ocalls.h"




int handle_ecall (unsigned long ecall_index, void * ecall_args, void * exit_target, void * untrusted_stack, void * enclave_base_addr)
{
	if (ecall_index < 0 || ecall_index >= ECALL_NR)
		return -1; 

	if (!enclave_base) {
		enclave_base = enclave_base_addr;
		enclave_top = (void *)((uint64_t) enclave_base_addr + GET_ENCLAVE_TLS(enclave_size));
	}   

	if (sgx_is_completely_within_enclave(exit_target, 0)) 
		return -1; 

	if (sgx_is_completely_within_enclave(untrusted_stack, 0)) 
		return -1; 

	SET_ENCLAVE_TLS(exit_target, exit_target);
	SET_ENCLAVE_TLS(ustack_top,  untrusted_stack);
	SET_ENCLAVE_TLS(ustack,      untrusted_stack);
	
	ocall_debugp(111);
	
	switch(ecall_index) {
		case ECALL_ENCLAVE_START:
			{
				ms_ecall_enclave_start_t * ms =
					(ms_ecall_enclave_start_t *) ecall_args;

				if (!ms) return -1;
				break;
			}
	}

	return 0;

}
