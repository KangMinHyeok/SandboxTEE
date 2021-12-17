#include <stdio.h>
#include <string.h>

#include "native_client/src/trusted/xcall/ecall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/service_runtime/sel_ldr.h"

int ecall_enclave_start (struct NaClApp * nap) {

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

	//memcpy(p, &(nap->dynamic_text_start), len);
	//p += len;
	//memcpy(p, &(nap->dynamic_text_end), len);
	//p += len;

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

	len = sizeof(int);
	memcpy(p, &(nap->bundle_size), len);
	p += len;

	memcpy(p, &(nap->main_exe_prevalidated), len);
	p += len;

	// TODO
	// struct NaClSyscallTableEntry syscall_table
	// struct NaClResourceNaClApp resources;
	// struct NaClVmmap mem_map;
	// struct NaClIntervalMultiset *mem_io_regions;
	// struct NaClDescEffector *effp;
	
	memcpy(p, &(nap->enable_dyncode_syscalls), len);
	p += len;
	memcpy(p, &(nap->use_shm_for_dynamic_text), len);
	p += len;

	// struct NaClDesc *text_shm;
	// struct NaclMutex dynamic_load_mutex;
	
	len = sizeof(uint8_t);
	memcpy(p, nap->dynamic_page_bitmap, len);
	p += len;
	
	len = sizeof(int);
	memcpy(p, &(nap->running), len);
	p += len;

	memcpy(p, &(nap->exit_status), len);
	p += len;

	// TODO
	// struct NaClValidationCache *validation_cache;
	
	memcpy(p, &(nap->ignore_validator_result), len);
	p += len;

	memcpy(p, &(nap->skip_validator), len);
	p += len;

	memcpy(p, &(nap->validator_stub_out_mode), len);
	p += len;

	len = sizeof(uintptr_t);
	memcpy(p, &(nap->break_addr), len);
	p += len;

	len = sizeof(uint32_t);
	memcpy(p, &(nap->exception_handler), len);
	p += len;

	len = sizeof(int);
	memcpy(p, &(nap->enable_exception_handling), len);
	p += len;

	memcpy(p, &(nap->enable_faulted_thread_queue), len);
	p += len;

	len = sizeof(int32_t);
	memcpy(p, &(nap->faulted_thread_count), len);
	p += len;

	len = sizeof(int);
	memcpy(p, &(nap->faulted_thread_fd_read), len);
	p += len;

	memcpy(p, &(nap->faulted_thread_fd_write), len);
	p += len;

	memcpy(p, &(nap->sc_nprocessors_onln), len);

	ms.nap = buf;
	
	return sgx_ecall(ECALL_ENCLAVE_START, &ms);
}

