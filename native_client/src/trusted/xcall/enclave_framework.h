#ifndef ENCLAVE_FRAMEWORK_H
#define ENCLAVE_FRAMEWORK_H

// TODO(mkpark) change the below headers
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

int sgx_ocall (unsigned long code, void * ms);
int sgx_ecall (unsigned long code, void * ms);

extern void * enclave_base, * enclave_top;

void * sgx_ocalloc (uint64_t size);
void sgx_ocfree (void);

bool sgx_is_completely_within_enclave(const void* addr, size_t size) ;
bool sgx_is_completely_outside_enclave(const void* addr, size_t size) ;
void* sgx_prepare_ustack(void);
void* sgx_alloc_on_ustack_aligned(size_t size, size_t alignment) ;
void* sgx_alloc_on_ustack(size_t size) ;
void* sgx_copy_to_ustack(const void* ptr, size_t size) ;
void sgx_reset_ustack(const void* old_ustack) ;
bool sgx_copy_ptr_to_enclave(void** ptr, void* uptr, size_t size) ;
bool sgx_copy_to_enclave(const void* ptr, size_t maxsize, const void* uptr, size_t usize) ;

uint32_t rdrand(void);
#endif
