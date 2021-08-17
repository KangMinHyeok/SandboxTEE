#ifndef __SGX_TLS_H__
#define __SGX_TLS_H__

#include <stddef.h>
#include "native_client/src/trusted/service_runtime/sgx/sgx_arch.h"

struct enclave_tls {
	uint64_t enclave_size;
	uint64_t tcs_offset;
	uint64_t initial_stack_offset;
	void *   aep;
	void *   ssa;
	sgx_arch_gpr_t * gpr;
	void *   exit_target;
	void *   fsbase;
	void *   stack;
	void *   ustack_top;
	void *   ustack;
	int is_main;
	// struct pal_handle_thread * thread;
	struct NaClThreadContext *nacl_current_thread;
};

#  define GET_ENCLAVE_TLS(member)                                   \
	__extension__ ({                                                              \
	 struct enclave_tls * tmp;                                   \
	 uint64_t val;                                               \
	 __asm__ ("movq %%gs:%c1, %q0": "=r" (val)                   \
			 : "i" (offsetof(struct enclave_tls, member)));         \
			 (__typeof(tmp->member)) val;                                \
			 })
#  define SET_ENCLAVE_TLS(member, value)                            \
	do {                                                            \
		__asm__ ("movq %q0, %%gs:%c1":: "r" (value),                \
				"i" (offsetof(struct enclave_tls, member)));           \
	} while (0)


/*
#  define GET_ENCLAVE_TLS(value, member)  \
		__extension__ ({  __asm__ ("movq %%gs:%c1, %q0": "=r" (value)      \
			 : "i" (member));  })

#  define SET_ENCLAVE_TLS(member, value)                            \
	do {                                                            \
		__asm__ ("movq %q0, %%gs:%c1":: "r" (value),                \
				"i" (offsetof(struct enclave_tls, member)));           \
	} while (0)
*/

#endif 
