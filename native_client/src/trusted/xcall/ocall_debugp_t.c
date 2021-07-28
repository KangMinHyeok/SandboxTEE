#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"

// TODO(mkpark): below struct should be changed
#include "/usr/include/errno.h"


int ocall_debugp (int val)
{

	ms_ocall_debugp_t * ms;
	void* old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_val = val;

	sgx_ocall(OCALL_DEBUGP, ms);
	sgx_reset_ustack(old_ustack);
	return 0;
}
