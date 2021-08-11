#include <stdio.h>
#include <string.h>

#include "native_client/src/trusted/xcall/ecall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/service_runtime/sel_ldr.h"

int ecall_enclave_start (struct NaClApp * nap) {

	struct NaClApp * test = nap;
	
	ms_ecall_enclave_start_t ms;
	char *buf = malloc(1024);
	char *p;
	int len;
	
	p = buf;

	len = sizeof(uint8_t);
	memcpy(p, &(nap->addr_bits), len);
	p += len;

	len = sizeof(uintptr_t);
	memcpy(p, &(nap->stack_size), len);
	p += len;

	len = sizeof(uint32_t);
	memcpy(p, &(nap->initial_nexe_max_code_bytes), len);
	p += len;

	len = sizeof(uintptr_t);
	memcpy(p, &(nap->mem_start), len);
	p += len;

#if NACL_ARCH(NACL_BUILD_ARCH) == NACL_x86 && NACL_BUILD_SUBARCH == 32
	memcpy(p, &(nap->pcrel_thunk), len);
	p += len;
	memcpy(p, &(nap->pcrel_thunk_end), len);
	p += len;
#endif
#if NACL_ARCH(NACL_BUILD_ARCH) == NACL_x86 && NACL_BUILD_SUBARCH == 64
	memcpy(p, &(nap->nacl_syscall_addr), len);
	p += len;
	memcpy(p, &(nap->get_tls_fast_path1_addr), len);
	p += len;
	memcpy(p, &(nap->get_tls_fast_path2_addr), len);
	p += len;
#endif
	
	memcpy(p, &(nap->static_text_end), len);
	p += len;

	memcpy(p, &(nap->dynamic_text_start), len);
	p += len;
	memcpy(p, &(nap->dynamic_text_end), len);
	p += len;

	memcpy(p, &(nap->rodata_start), len);
	p += len;

	memcpy(p, &(nap->data_start), len);
	p += len;
	memcpy(p, &(nap->data_end), len);
	p += len;

	memcpy(p, &(nap->initial_entry_pt), len);
	p += len;
	memcpy(p, &(nap->user_entry_pt), len);
	p += len;
	memcpy(p, &(nap->user_entry_pt), len);
	p += len;

	ms.nap = 0;

	test = NULL;

	

	return sgx_ecall(ECALL_ENCLAVE_START, &ms);
}

