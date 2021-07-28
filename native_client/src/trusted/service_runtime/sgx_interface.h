#ifndef _SGX_INTERFACE_
#define _SGX_INTERFACE_
#include "native_client/src/trusted/service_runtime/sgx/sgx_arch.h"
#include "native_client/src/trusted/service_runtime/sel_ldr.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_common.h"

// driver path
#include "sgx_user.h"

#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SE_LEAF 0x12


#define SGX_FLAGS_INITTED        0x0000000000000001ULL 



enum sgx_page_type { SGX_PAGE_SECS, SGX_PAGE_TCS, SGX_PAGE_REG };

int add_pages_to_enclave(sgx_arch_secs_t * secs,
						 void * addr, void * user_addr,
						 unsigned long size,
						 enum sgx_page_type type, int prot,
						 bool skip_eextend,
						 const char * comment);

int load_enclave(struct NaClApp *nap);

int init_enclave(struct NaClApp *nap);

#endif
