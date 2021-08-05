

#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/ecall_types.h"


int ocall_cpuid (unsigned int leaf, unsigned int subleaf, unsigned int values[4]) {
	int retval = 0;
	ms_ocall_cpuid_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_leaf = leaf;
	ms->ms_subleaf = subleaf;

	retval = sgx_ocall(OCALL_CPUID, ms);
	if (!retval) {
		values[0] = ms->ms_values[0];
		values[1] = ms->ms_values[1];
		values[2] = ms->ms_values[2];
		values[3] = ms->ms_values[3];
	}

	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_getpid () {
	int retval = 0;
	ms_ocall_pid_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	retval = sgx_ocall(OCALL_GETPID, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fionread (int fd) {
	int retval = 0;
	ms_ocall_fionread_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;

	retval = sgx_ocall(OCALL_FIONREAD, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_print_string (char * str, unsigned int length) {   
	int retval = 0;
	ms_ocall_print_string_t * ms;
	void * old_ustack = sgx_prepare_ustack();

	if (!str || length <= 0) {
		goto out;
		retval = -1;
	}

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));
	if (!ms) { 
		retval = -1;
		goto out;
	}

	ms->ms_str = sgx_copy_to_ustack(str, length);
	ms->ms_length = length;
	ms->ms_str = sgx_copy_to_ustack(str, length);

	retval = sgx_ocall(OCALL_PRINT_STRING, ms);

	if (retval > 0 && (size_t) retval > length){
		retval = -1;
		goto out;
	}

out: 
	sgx_reset_ustack(old_ustack);
	return retval;
}

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

