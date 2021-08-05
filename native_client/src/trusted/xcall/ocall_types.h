#ifndef SGXLIB_OCALL_TYPES_H
#define SGXLIB_OCALL_TYPES_H

#include "native_client/src/include/build_config.h"

#if NACL_SGX == 0 && NACL_USGX == 0
#error "This file must be compiled with NACL_SGX or NACL_USGX"
#endif

#define OCALL_NO_TIMEOUT   ((unsigned long) -1)

enum {
	OCALL_EXIT = 0,
	//
	OCALL_ALLOC_UNTRUSTED,	// Mem
	OCALL_MMAP_UNTRUSTED,		// Mem
	OCALL_UNMAP_UNTRUSTED,	// Mem
	OCALL_MPROTECT,					// Mem
	//
	OCALL_CPUID,						// Misc
	OCALL_GETPID, 					// Misc
	OCALL_FIONREAD,					// Misc
	OCALL_FUTEX,						// Misc
	OCALL_PRINT_STRING, 		// Misc
	OCALL_DEBUGP,						// Misc
	//
	OCALL_OPEN,							// File
	OCALL_CLOSE,						// File
	OCALL_READ,							// File
	OCALL_WRITE,						// File
	//
	// OCALL_FSEEK,
	// OCALL_FTELL,
	// OCALL_FFLUSH,
	OCALL_STAT,							// File
	OCALL_LSTAT,						// File
	OCALL_FSTAT,						// File
	OCALL_FSETNONBLOCK,			// File
	OCALL_FCHMOD, 					// File
	OCALL_FCHDIR, 					// File 
	OCALL_FSYNC, 						// File
	OCALL_FDATASYNC, 				// File
	OCALL_FTRUNCATE,				// File
	OCALL_LSEEK64, 					// File
	OCALL_PREAD64,					// File
	OCALL_PWRITE64,					// File
	//
	OCALL_MKDIR,						// OS
	OCALL_RMDIR,						// OS
	OCALL_CHDIR,						// OS
	OCALL_GETCWD,						// OS 
	OCALL_LINK,							// OS
	OCALL_UNLINK,						// OS
	OCALL_TRUNCATE,					// OS 
	OCALL_RENAME,						// OS 
	OCALL_CHMOD,						// OS 
	OCALL_ACCESS,						// OS 
	OCALL_FCNTL,						// OS 	
	OCALL_GETDENTS,					// OS 
	// 
	OCALL_SOCKETPAIR,				// Socket
	OCALL_SOCK_LISTEN,			// Socket
	OCALL_SOCK_ACCEPT,			// Socket
	OCALL_SOCK_CONNECT,			// Socket
	OCALL_SOCK_RECV,				// Socket
	OCALL_SOCK_SEND, 				// Socket		// TODO MCAST
	OCALL_SOCK_RECV_FD,			// Socket
	OCALL_SOCK_SEND_FD,			// Socket
	OCALL_SOCK_SETOPT,			// Socket
	OCALL_SOCK_SHUTDOWN,		// Socket
	//
	OCALL_GETTIMEOFDAY, 		// Time
	OCALL_CLOCK_GETTIME, 		// Time
	OCALL_SLEEP, 						// Time
	OCALL_NSLEEP, 					// Time
	//OCALL_POLL,	
	//OCALL_DELETE,

	OCALL_NR,

};

// Socket
typedef struct {
	int ms_domain, ms_type, ms_protocol;
	int ms_sockfds[2];
} ms_ocall_socketpair_t;

typedef struct {
	int ms_domain, ms_type, ms_protocol;
	struct sockaddr * ms_addr;
	unsigned int ms_addrlen;
	//struct sockopt ms_sockopt;
} ms_ocall_sock_listen_t;

typedef struct {
	int ms_sockfd;
	struct sockaddr * ms_addr;
	unsigned int ms_addrlen;
	//struct sockopt ms_sockopt;
} ms_ocall_sock_accept_t;

typedef struct {
	int ms_domain, ms_type, ms_protocol;
	struct sockaddr * ms_addr;
	unsigned int ms_addrlen;
	struct sockaddr * ms_bind_addr;
	unsigned int ms_bind_addrlen;
	//struct sockopt ms_sockopt;
} ms_ocall_sock_connect_t;

typedef struct {
	int ms_sockfd;
	void * ms_buf;
	unsigned int ms_count;
	struct sockaddr * ms_addr;
	unsigned int ms_addrlen;
} ms_ocall_sock_recv_t;

typedef struct {
	int ms_sockfd;
	void * ms_buf;
	unsigned int ms_count;
	struct sockaddr * ms_addr;
	unsigned int ms_addrlen;
} ms_ocall_sock_send_t;

typedef struct {
	int ms_sockfd;
	void * ms_buf;
	unsigned int ms_count;
	unsigned int * ms_fds;
	unsigned int ms_nfds;
} ms_ocall_sock_recv_fd_t;

typedef struct {
	int ms_sockfd;
	void * ms_buf;
	unsigned int ms_count;
	unsigned int * ms_fds;
	unsigned int ms_nfds;
} ms_ocall_sock_send_fd_t;

typedef struct {
	int ms_sockfd;
	int ms_level;
	int ms_optname;
	void * ms_optval;
	unsigned int ms_optlen;
} ms_ocall_sock_setopt_t;

typedef struct {
	int ms_sockfd;
	int ms_how;
} ms_ocall_sock_shutdown_t;


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

// Futex
typedef struct {
	int * ms_futex;
	int ms_op, ms_val;
	unsigned long ms_timeout;
	int * ms_uaddr2;
	int ms_val3;
} ms_ocall_futex_t;





// Futex
int sgx_ocall_futex(void * pms);

// Socket
int sgx_ocall_socketpair(void * pms);
int sgx_ocall_sock_listen(void * pms);
int sgx_ocall_sock_accept(void * pms);
int sgx_ocall_sock_connect(void * pms);
int sgx_ocall_sock_recv(void * pms);
int sgx_ocall_sock_send(void * pms);
int sgx_ocall_sock_recv_fd(void * pms);
int sgx_ocall_sock_send_fd(void * pms);
int sgx_ocall_sock_setopt(void * pms);
int sgx_ocall_sock_shutdown(void * pms);

// Time
int sgx_ocall_gettimeofday(void * pms);
int sgx_ocall_clock_gettime(void * pms);
int sgx_ocall_sleep(void * pms);
int sgx_ocall_nsleep(void * pms);

// Debug
int sgx_ocall_debugp(void * pms);


#endif
