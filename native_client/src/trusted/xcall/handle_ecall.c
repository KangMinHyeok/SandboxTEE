
#include "native_client/src/include/build_config.h"


#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/ecall_types.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_tls.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"

#include "native_client/src/trusted/service_runtime/sel_ldr.h"
#include "native_client/src/trusted/service_runtime/sel_main.h"



int handle_ecall (unsigned long ecall_index, void * ecall_args, void * exit_target, void * untrusted_stack, void * enclave_base_addr)
{
	if (ecall_index < 0 || ecall_index >= ECALL_NR)
		return -1; 

	if (!enclave_base) {
		enclave_base = enclave_base_addr;
		enclave_top = (void *)((uint64_t) enclave_base_addr + GET_ENCLAVE_TLS(enclave_size));
	}   

	if (sgx_is_completely_within_enclave(exit_target, 0)) 
		return -1; 

	if (sgx_is_completely_within_enclave(untrusted_stack, 0)) 
		return -1; 

	SET_ENCLAVE_TLS(exit_target, exit_target);
	SET_ENCLAVE_TLS(ustack_top,  untrusted_stack);
	SET_ENCLAVE_TLS(ustack,      untrusted_stack);
	
	//ocall_debugp(111);
	
	switch(ecall_index) {
		case ECALL_ENCLAVE_START:
			{
				ms_ecall_enclave_start_t * ms =
					(ms_ecall_enclave_start_t *) ecall_args;
				if (!ms) return -1;

  			struct SelLdrOptions          optionsImpl;
  			struct SelLdrOptions          *options = &optionsImpl;

  			struct NaClApp nap;

  			// TODO (mkpark): copy nap from usgx to sgx
  			char *nap_p;
  			int len;

				nap_p = ms->nap;
				
				len = sizeof(uint8_t);
				sgx_copy_to_enclave(&nap.addr_bits, len, nap_p, len);
				nap_p += len;

				len = sizeof(uintptr_t);
				sgx_copy_to_enclave(&nap.stack_size, len, nap_p, len);
				nap_p += len;

				len = sizeof(uint32_t);
				sgx_copy_to_enclave(&nap.initial_nexe_max_code_bytes, len, nap_p, len);
				nap_p += len;

				len = sizeof(uintptr_t);
				sgx_copy_to_enclave(&nap.mem_start, len, nap_p, len);
				nap_p += len;

#if NACL_ARCH(NACL_BUILD_ARCH) == NACL_x86 && NACL_BUILD_SUBARCH == 64
				sgx_copy_to_enclave(&nap.nacl_syscall_addr, len, nap_p, len);
				nap_p += len;
				sgx_copy_to_enclave(&nap.get_tls_fast_path1_addr, len, nap_p, len);
				nap_p += len;
				sgx_copy_to_enclave(&nap.get_tls_fast_path2_addr, len, nap_p, len);
				nap_p += len;
#endif
				sgx_copy_to_enclave(&nap.static_text_end, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.rodata_start, len, nap_p, len);
				nap_p += len;
				sgx_copy_to_enclave(&nap.data_start, len, nap_p, len);
				nap_p += len;
				sgx_copy_to_enclave(&nap.data_end, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.initial_entry_pt, len, nap_p, len);
				nap_p += len;
				sgx_copy_to_enclave(&nap.user_entry_pt, len, nap_p, len);
				nap_p += len;

				len = sizeof(int);
				sgx_copy_to_enclave(&nap.bundle_size, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.main_exe_prevalidated, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.enable_dyncode_syscalls, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.use_shm_for_dynamic_text, len, nap_p, len);
				nap_p += len;

				len = sizeof(uint8_t);
				sgx_copy_to_enclave(&nap.dynamic_page_bitmap, len, nap_p, len);
				nap_p += len;

				len = sizeof(int);
				sgx_copy_to_enclave(&nap.running, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.exit_status, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.ignore_validator_result, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.skip_validator, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.validator_stub_out_mode, len, nap_p, len);
				nap_p += len;

				len = sizeof(uintptr_t);
				sgx_copy_to_enclave(&nap.break_addr, len, nap_p, len);
				nap_p += len;

				len = sizeof(uint32_t);
				sgx_copy_to_enclave(&nap.exception_handler, len, nap_p, len);
				nap_p += len;

				len = sizeof(int);
				sgx_copy_to_enclave(&nap.enable_exception_handling, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.enable_faulted_thread_queue, len, nap_p, len);
				nap_p += len;

				len = sizeof(uint32_t);
				sgx_copy_to_enclave(&nap.faulted_thread_count, len, nap_p, len);
				nap_p += len;

				len = sizeof(int);
				sgx_copy_to_enclave(&nap.faulted_thread_fd_read, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.faulted_thread_fd_write, len, nap_p, len);
				nap_p += len;

				sgx_copy_to_enclave(&nap.sc_nprocessors_onln, len, nap_p, len);

  			// TODO (mkpark): copy options from usgx to sgx <- needed?

				NaClAppPrepareModuleInSGX(options, &nap);
				break;
			}
	}

	return 0;

}
