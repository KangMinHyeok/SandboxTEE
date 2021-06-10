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
	struct pal_handle_thread * thread;
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

/* update these constant according to struct enclave_tls */
#define SGX_ENCLAVE_SIZE            0x00
#define SGX_TCS_OFFSET              0x08
#define SGX_INITIAL_STACK_OFFSET    0x10
#define SGX_AEP                     0x18
#define SGX_SSA                     0x20
#define SGX_GPR                     0x28
#define SGX_EXIT_TARGET             0x30
#define SGX_FSBASE                  0x38
#define SGX_STACK                   0x40
#define SGX_USTACK_TOP              0x48
#define SGX_USTACK                  0x50
#define SGX_ISMAIN                  0x58
#define SGX_THREAD                  0x5B


#endif 
