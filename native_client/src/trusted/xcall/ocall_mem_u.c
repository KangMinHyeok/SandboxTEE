#include <stddef.h>
#include <sys/mman.h>


#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_common.h"

int sgx_ocall_alloc_untrusted(void * pms) {
	ms_ocall_alloc_untrusted_t * ms = (ms_ocall_alloc_untrusted_t *) pms;
	void * addr;
	
	addr = (void *) mmap(NULL, ms->ms_size,	PROT_READ|PROT_WRITE,	MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if ((long)addr < 0)
		return -1;

	ms->ms_mem = addr;
	return 0;
}

int sgx_ocall_mmap_untrusted(void * pms) {
	ms_ocall_mmap_untrusted_t * ms = (ms_ocall_mmap_untrusted_t *) pms;
	void * addr;
	addr = (void *) mmap(NULL, ms->ms_size, ms->ms_prot, MAP_FILE|MAP_SHARED,	ms->ms_fd, ms->ms_offset);
	if ((long)addr < 0)
		return -1;

	ms->ms_mem = addr;
	return 0;
}

int sgx_ocall_unmap_untrusted(void * pms) {
	ms_ocall_unmap_untrusted_t * ms = (ms_ocall_unmap_untrusted_t *) pms;
	munmap((void *)ALLOC_ALIGNDOWN(ms->ms_mem), ALLOC_ALIGNUP((long)(ms->ms_mem) + ms->ms_size) -	ALLOC_ALIGNDOWN(ms->ms_mem));
	return 0;
}

int sgx_ocall_mprotect(void * pms) {
	ms_ocall_mprotect_t * ms = (ms_ocall_mprotect_t *) pms;
	int ret = 0;
	ret = mprotect((void *)ms->ms_mem, ms->ms_size, ms->ms_prot);
	return ret;
}


