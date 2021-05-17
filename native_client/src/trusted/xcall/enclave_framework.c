#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>



int sgx_ocall (unsigned long code, void * ms) {

	return 0;
}

void * sgx_ocalloc (uint64_t size) {

	return NULL;
}


void sgx_ocfree (void) {

}

bool sgx_is_completely_within_enclave(const void* addr, size_t size) {

	return true;
}

 
bool sgx_is_completely_outside_enclave(const void* addr, size_t size) {

	return true;
}


void* sgx_prepare_ustack(void) {

	return NULL;
}


void* sgx_alloc_on_ustack_aligned(size_t size, size_t alignment) {

	return NULL;
}


void* sgx_alloc_on_ustack(size_t size) {

	return NULL;
}


void* sgx_copy_to_ustack(const void* ptr, size_t size) {

	return NULL;
}

void sgx_reset_ustack(const void* old_ustack) {

}

bool sgx_copy_ptr_to_enclave(void** ptr, void* uptr, size_t size) {

	return true;
}

bool sgx_copy_to_enclave(const void* ptr, size_t maxsize, const void* uptr,
		        size_t usize) {

	return true;
}


