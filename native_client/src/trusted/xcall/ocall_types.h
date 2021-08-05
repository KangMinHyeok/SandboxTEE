#ifndef SGXLIB_OCALL_TYPES_H
#define SGXLIB_OCALL_TYPES_H

#include "native_client/src/include/build_config.h"

#if NACL_SGX == 0 && NACL_USGX == 0
#error "This file must be compiled with NACL_SGX or NACL_USGX"
#endif


enum {
	OCALL_EXIT = 0,
	//
	OCALL_PRINT_STRING,
	OCALL_ALLOC_UNTRUSTED,
	OCALL_MMAP_UNTRUSTED,
	OCALL_UNMAP_UNTRUSTED,
	OCALL_MPROTECT,
	//
	OCALL_CPUID,
	//
	OCALL_OPEN,
	OCALL_CLOSE,
	OCALL_READ,
	OCALL_WRITE,
	//
	OCALL_GETPID, 
	// FILE
	// OCALL_FSEEK,
	// OCALL_FTELL,
	// OCALL_FFLUSH,
	OCALL_STAT,
	OCALL_LSTAT,
	OCALL_FSTAT,
	OCALL_FIONREAD,
	OCALL_FSETNONBLOCK,
	OCALL_FCHMOD, // 
	OCALL_FCHDIR, // 
	OCALL_FSYNC, //
	OCALL_FDATASYNC, //
	OCALL_FTRUNCATE,
	OCALL_LSEEK64, 
	OCALL_PREAD64,
	OCALL_PWRITE64,
	OCALL_MKDIR,
	OCALL_RMDIR,
	OCALL_CHDIR,

	OCALL_GETCWD,
	OCALL_LINK,
	OCALL_UNLINK,
	OCALL_TRUNCATE,
	OCALL_RENAME,
	OCALL_CHMOD,
	OCALL_ACCESS,
	OCALL_FCNTL,
	OCALL_GETDENTS,
	//
	OCALL_FUTEX,
	// 
	OCALL_SOCKETPAIR,
	OCALL_SOCK_LISTEN,
	OCALL_SOCK_ACCEPT,
	OCALL_SOCK_CONNECT,
	OCALL_SOCK_RECV,
	OCALL_SOCK_SEND, // TODO MCAST
	OCALL_SOCK_RECV_FD,
	OCALL_SOCK_SEND_FD,
	OCALL_SOCK_SETOPT,
	OCALL_SOCK_SHUTDOWN,
	//
	OCALL_GETTIMEOFDAY, 	// Time
	OCALL_CLOCK_GETTIME, 	// Time
	OCALL_SLEEP, 					// Time
	OCALL_NSLEEP, 				// Time
	//OCALL_POLL,	
	//OCALL_DELETE,

	OCALL_DEBUGP,

	OCALL_NR,

};

// Time
typedef struct {
	long ms_tv_sec;
	long ms_tv_usec;
} ms_ocall_gettimeofday_t;

typedef struct {
	int ms_clk_id;
	long ms_tp_sec;
	long ms_tp_nsec;
} ms_ocall_clock_gettime_t;

typedef struct {
	unsigned long ms_microsec;
} ms_ocall_sleep_t;

typedef struct {
	long ms_rqtp_sec;
	long ms_rqtp_nsec;
	long ms_rmtp_sec;
	long ms_rmtp_nsec;
} ms_ocall_nsleep_t;

typedef struct {
	int ms_val;
} ms_ocall_debugp_t;



// Time
int sgx_ocall_gettimeofday(void * pms);
int sgx_ocall_clock_gettime(void * pms);
int sgx_ocall_sleep(void * pms);
int sgx_ocall_nsleep(void * pms);

int sgx_ocall_debugp(void * pms);


#endif
