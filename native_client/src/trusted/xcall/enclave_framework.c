#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "native_client/src/trusted/service_runtime/sgx/sgx_tls.h"

#define RED_ZONE_SIZE 128

// TODO need to remove
#define IS_POWER_OF_2(x) \
	__extension__ ({ assert((x) != 0); \
	 (((x) & ((x) - 1)) == 0); })

#define ALIGN_DOWN_POW2(val, alignment) \
	((val) - ((val) & ((alignment) - 1))) // `~` doesn't work if `alignment` is of a smaller type

#define ALIGN_DOWN_PTR_POW2(ptr, alignment) ((__typeof__(ptr))(ALIGN_DOWN_POW2((uintptr_t)(ptr), \
				alignment)))

void * enclave_base, * enclave_top;

/*
void * sgx_ocalloc (uint64_t size) {
	void * ustack = GET_ENCLAVE_TLS(ustack) - size;
	SET_ENCLAVE_TLS(ustack, ustack);
	return ustack;
}


void sgx_ocfree (void) {
	SET_ENCLAVE_TLS(ustack, GET_ENCLAVE_TLS(ustack_top));
}
*/

bool sgx_is_completely_within_enclave(const void* addr, size_t size) {
	if ((uintptr_t)addr > UINTPTR_MAX - size) {
		return false;
	}

	return enclave_base <= addr && (uint64_t) addr + size <= (uint64_t) enclave_top;
}


bool sgx_is_completely_outside_enclave(const void* addr, size_t size) {
	if ((uintptr_t)addr > UINTPTR_MAX - size) {
		return false;
	}

	return (uint64_t) enclave_base >= (uint64_t) addr + size || enclave_top <= addr;
}


void* sgx_prepare_ustack(void) {
	void* old_ustack = GET_ENCLAVE_TLS(ustack);

	void* ustack = old_ustack;
	uint64_t ustack_tmp;
	if (ustack != GET_ENCLAVE_TLS(ustack_top)) {
		ustack_tmp = (uint64_t) ustack - RED_ZONE_SIZE;
		ustack = (void *) ustack_tmp;
		//ustack -= RED_ZONE_SIZE;
	}
	SET_ENCLAVE_TLS(ustack, ustack);

	return old_ustack;
}


void* sgx_alloc_on_ustack_aligned(size_t size, size_t alignment) {

	assert(IS_POWER_OF_2(alignment));
	void* ustack = (void*)((uint64_t) GET_ENCLAVE_TLS(ustack) - size);
	ustack = ALIGN_DOWN_PTR_POW2(ustack, alignment);
	if (!sgx_is_completely_outside_enclave(ustack, size)) {
		return NULL;
	}
	SET_ENCLAVE_TLS(ustack, ustack);
	return ustack;
}


void* sgx_alloc_on_ustack(size_t size) {
	return sgx_alloc_on_ustack_aligned(size, 1);
}


void* sgx_copy_to_ustack(const void* ptr, size_t size) {
	if (!sgx_is_completely_within_enclave(ptr, size)) {
		return NULL;    }
	void* uptr = sgx_alloc_on_ustack(size);
	if (uptr) {        memcpy(uptr, ptr, size);
	}    return uptr;
}

void sgx_reset_ustack(const void* old_ustack) {
	assert(old_ustack <= GET_ENCLAVE_TLS(ustack_top));
	SET_ENCLAVE_TLS(ustack, old_ustack);
}

bool sgx_copy_ptr_to_enclave(void** ptr, void* uptr, size_t size) {
	assert(ptr);
	if (!sgx_is_completely_outside_enclave(uptr, size)) {
		*ptr = NULL;
		return false;
	}
	*ptr = uptr;
	return true;

}

bool sgx_copy_to_enclave(const void* ptr, size_t maxsize, const void* uptr,
		size_t usize) {
	if (usize > maxsize ||
			!sgx_is_completely_outside_enclave(uptr, usize) ||
			!sgx_is_completely_within_enclave(ptr, usize)) {
		return false;
	}
	memcpy((void*)ptr, uptr, usize);
	return true;
}


