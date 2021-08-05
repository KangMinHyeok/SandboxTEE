#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/ecall_types.h"

int ocall_futex (int * futex, int op, int val, uint64_t * timeout, int * uaddr2, int val3) {
	int retval = 0;
	ms_ocall_futex_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	if (sgx_is_completely_within_enclave(futex, sizeof(int))) {
		sgx_reset_ustack(old_ustack);
		return -1;
	}

	ms->ms_futex = futex;
	ms->ms_op = op;
	ms->ms_val = val;
	ms->ms_timeout = timeout ? *timeout : OCALL_NO_TIMEOUT;
	ms->ms_uaddr2 = uaddr2;
	ms->ms_val3 = val3;

	retval = sgx_ocall(OCALL_FUTEX, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

