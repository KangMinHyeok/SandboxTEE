#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"




int ocall_alloc_untrusted (uint64_t size, void ** mem) {
	int retval = 0;
	ms_ocall_alloc_untrusted_t  *ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_size = size;

	retval = sgx_ocall(OCALL_ALLOC_UNTRUSTED, ms);
	if (!retval) {
		if (sgx_is_completely_within_enclave(ms->ms_mem, size)) {
			sgx_reset_ustack(old_ustack);
			return -1;
		}
		*mem = ms->ms_mem;
	}
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_mmap_untrusted (int fd, uint64_t offset, uint64_t size, unsigned short prot, void ** mem) {
	int retval = 0;
	ms_ocall_mmap_untrusted_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_offset = offset;
	ms->ms_size = size;
	ms->ms_prot = prot;

	retval = sgx_ocall(OCALL_MMAP_UNTRUSTED, ms);
	if (!retval) {
		if (sgx_is_completely_within_enclave(ms->ms_mem, size)) {
			sgx_reset_ustack(old_ustack);
			return -1;
		}
		*mem = ms->ms_mem;
	}
	sgx_reset_ustack(old_ustack);

	return retval;
}

int ocall_unmap_untrusted (void * mem, uint64_t size) {
	int retval = 0;
	void * old_ustack = sgx_prepare_ustack();
	ms_ocall_unmap_untrusted_t *ms;

	if (sgx_is_completely_within_enclave(mem, size)) {
		sgx_reset_ustack(old_ustack);
		return -1;
	}

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));
	ms->ms_mem  = mem;
	ms->ms_size = size;

	retval = sgx_ocall(OCALL_UNMAP_UNTRUSTED, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_mprotect (void *addr, size_t len, int prot) {
	int retval = 0;
	void * old_ustack = sgx_prepare_ustack();
	ms_ocall_mprotect_t *ms;

	if (sgx_is_completely_within_enclave(addr, len)) {
		sgx_reset_ustack(old_ustack);
		return -1;
	}

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));
	ms->ms_mem = addr;
	ms->ms_size = len;
	ms->ms_prot = prot;

	retval = sgx_ocall(OCALL_MPROTECT, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}


