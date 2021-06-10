#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"

// TODO(mkpark): below struct should be changed
#include "/usr/include/errno.h"


int ocall_debugp (int val)
{

	int retval = 0;
	ms_ocall_debugp_t * ms;
	void* old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	retval = sgx_ocall(OCALL_DEBUGP, ms);
	if (!retval) {
		val = ms->ms_val;
	}
	sgx_reset_ustack(old_ustack);
	return retval;
}
