#ifndef _SGX_INTERFACE_
#define _SGX_INTERFACE_
#include "sgx/sgx_arch.h"
#include "sel_ldr.h"

#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SE_LEAF 0x12

#define PRESET_PAGESIZE (1 << 12)

#define SGX_FLAGS_INITTED        0x0000000000000001ULL 

#define SGX_MAGIC 0xA4

#define SGX_IOC_ENCLAVE_CREATE \
	_IOW(SGX_MAGIC, 0x00, struct sgx_enclave_create)
#define SGX_IOC_ENCLAVE_ADD_PAGE \
	_IOW(SGX_MAGIC, 0x01, struct sgx_enclave_add_page)

#undef ALLOC_ALIGNUP
#define ALLOC_ALIGNUP(addr) \
	(pagesize ? (((unsigned long) (addr)) + pageshift) & pagemask : (unsigned long) (addr))

#undef INTERNAL_SYSCALL_ERROR
#define INTERNAL_SYSCALL_ERROR(val) ((val) < 0)

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val) ((unsigned long) (val) >= (unsigned long)-4095L)

#define IS_ERR INTERNAL_SYSCALL_ERROR
#define IS_ERR_P INTERNAL_SYSCALL_ERROR_P

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val) (-(val))

#undef INTERNAL_SYSCALL_ERRNO_P
#define INTERNAL_SYSCALL_ERRNO_P(val) (-((long) val))

#define ERRNO_P INTERNAL_SYSCALL_ERRNO_P
#define ERRNO INTERNAL_SYSCALL_ERRNO


enum sgx_page_type { SGX_PAGE_SECS, SGX_PAGE_TCS, SGX_PAGE_REG };

struct sgx_enclave_create {
	uint64_t src;
};

struct sgx_enclave_add_page {
	uint64_t addr;
	uint64_t src;
	uint64_t secinfo;
	uint16_t mrmask;
};


int add_pages_to_enclave(sgx_arch_secs_t * secs,
						 void * addr, void * user_addr,
						 unsigned long size,
						 enum sgx_page_type type, int prot,
						 bool skip_eextend,
						 const char * comment);

int init_enclave(struct NaClApp *nap);



#endif
