#ifndef SGXLIB_OCALL_TYPES_H
#define SGXLIB_OCALL_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "native_client/src/include/build_config.h"

#if NACL_SGX == 0 && NACL_USGX == 0
#error "This file must be compiled with NACL_SGX or NACL_USGX"
#endif

#define OCALL_NO_TIMEOUT   ((unsigned long) -1)

enum {
	//
	OCALL_ALLOC_UNTRUSTED,	// Mem
	OCALL_MMAP_UNTRUSTED,		// Mem
	OCALL_UNMAP_UNTRUSTED,	// Mem
	OCALL_MPROTECT,					// Mem
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
	OCALL_DUP,							// File
	//
	OCALL_MKDIR,						// Dir
	OCALL_RMDIR,						// Dir
	OCALL_CHDIR,						// Dir
	OCALL_GETCWD,						// Dir
	OCALL_LINK,							// Dir
	OCALL_UNLINK,						// Dir
	OCALL_TRUNCATE,					// Dir
	OCALL_RENAME,						// Dir
	OCALL_CHMOD,						// Dir
	OCALL_ACCESS,						// Dir
	OCALL_FCNTL,						// Dir	
	OCALL_GETDENTS,					// Dir 
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
	//
	OCALL_EXIT,							// Misc
	OCALL_CPUID,						// Misc
	OCALL_GETPID, 					// Misc
	OCALL_FIONREAD,					// Misc
	OCALL_FUTEX,						// Misc
	OCALL_PRINT_STRING, 		// Misc
	OCALL_DEBUGP,						// Misc
	
	
	OCALL_NR,

};


// Mem
typedef struct {
	unsigned long ms_size;
	void * ms_mem;
} ms_ocall_alloc_untrusted_t;

typedef struct {
	int ms_fd;
	unsigned long ms_offset;
	unsigned long ms_size;
	unsigned short ms_prot;
	void * ms_mem;
} ms_ocall_mmap_untrusted_t;

typedef struct {
	const void * ms_mem;
	unsigned long ms_size;
} ms_ocall_unmap_untrusted_t;

typedef struct {
	const void * ms_mem;
	size_t ms_size;
	int ms_prot;
} ms_ocall_mprotect_t;


// File
typedef struct {
	const char * ms_pathname;
	int ms_pathlen;
	int ms_flags;
	unsigned short ms_mode;
} ms_ocall_open_t;

typedef struct {
	int ms_fd;
} ms_ocall_close_t;

typedef struct {
	int ms_fd;
	void * ms_buf;
	unsigned int ms_count;
} ms_ocall_read_t;

typedef struct {
	int ms_fd;
	const void * ms_buf;
	unsigned int ms_count;
} ms_ocall_write_t;

typedef struct {
	const void * ms_path;
	struct stat * ms_stat;
} ms_ocall_stat_t;

typedef struct {
	const void * ms_path;
	struct stat * ms_stat;
} ms_ocall_lstat_t;

typedef struct {
	int ms_fd;
	struct stat * ms_stat;
} ms_ocall_fstat_t;

typedef struct {
	int ms_fd;
	int ms_nonblocking;
} ms_ocall_fsetnonblock_t;

typedef struct {
	int ms_fd;
	unsigned short ms_mode;
} ms_ocall_fchmod_t;

typedef struct {
	int ms_fd;
} ms_ocall_fsync_t, ms_ocall_fchdir_t, ms_ocall_fdatasync_t;

typedef struct {
	int ms_fd;
	uint64_t ms_length;
} ms_ocall_ftruncate_t;

typedef struct {
	int ms_fd;
	long int ms_offset;
	int ms_whence;
} ms_ocall_lseek64_t;

typedef struct {
	int ms_fd;
	void * ms_buf;
	unsigned int ms_count;
	unsigned long ms_pos;
} ms_ocall_pread64_t;

typedef struct {
	int ms_fd;
	const void * ms_buf;
	unsigned int ms_count;
	unsigned int ms_offset;
} ms_ocall_pwrite64_t;

typedef struct {
	int ms_fd;
} ms_ocall_dup_t;

// Dir
typedef struct {
	const char * ms_pathname;
	unsigned short ms_mode;
} ms_ocall_mkdir_t;

typedef struct {
	const char * ms_pathname;
} ms_ocall_rmdir_t;

typedef struct {
	const char * ms_pathname;
} ms_ocall_chdir_t;

typedef struct {
	char * ms_path;
	size_t ms_pathlen;
	size_t ms_size;
} ms_ocall_getcwd_t;

typedef struct {
	const char * ms_oldpath;
	const char * ms_newpath;
} ms_ocall_link_t;

typedef struct {
	const char * ms_pathname;
} ms_ocall_unlink_t;

typedef struct {
	const char * ms_pathname;
	unsigned int ms_size;
} ms_ocall_truncate_t;

typedef struct {
	const char * ms_oldpath;
	const char * ms_newpath;
} ms_ocall_rename_t;

typedef struct {
	const char * ms_path;
	uint64_t ms_mode;
} ms_ocall_chmod_t;

typedef struct {
	const char * ms_pathname;
	unsigned short ms_mode;
} ms_ocall_access_t;

typedef struct {
	int ms_fd;
	int ms_cmd;
	long ms_args;
} ms_ocall_fcntl_t;

typedef struct {
	int ms_fd;
	struct linux_dirent64 * ms_dirp;
	unsigned int ms_size;
} ms_ocall_getdents_t;


// Socket
typedef struct {
	int ms_domain, ms_type, ms_protocol;
	int ms_sockfds[2];
} ms_ocall_socketpair_t;

typedef struct {
	int ms_domain, ms_type, ms_protocol;
	const struct sockaddr * ms_addr;
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
  const	struct sockaddr * ms_addr;
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
	const void * ms_buf;
	unsigned int ms_count;
	const struct sockaddr * ms_addr;
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
	const void * ms_buf;
	unsigned int ms_count;
	const unsigned int * ms_fds;
	unsigned int ms_nfds;
} ms_ocall_sock_send_fd_t;

typedef struct {
	int ms_sockfd;
	int ms_level;
	int ms_optname;
	const void * ms_optval;
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

// Misc
typedef struct {
	int tmp;
} ms_ocall_pid_t;

typedef struct {
	unsigned int ms_leaf;
	unsigned int ms_subleaf;
	unsigned int ms_values[4];
} ms_ocall_cpuid_t;

typedef struct {
	int ms_fd;
} ms_ocall_fionread_t;

typedef struct {
	int * ms_futex;
	int ms_op, ms_val;
	unsigned long ms_timeout;
	int * ms_uaddr2;
	int ms_val3;
} ms_ocall_futex_t;

typedef struct {
	const char * ms_str;
	unsigned int ms_length;
} ms_ocall_print_string_t;

typedef struct {
	int ms_val;
} ms_ocall_debugp_t;


// File
int sgx_ocall_open(void * pms);
int sgx_ocall_close(void * pms);
int sgx_ocall_read(void * pms);
int sgx_ocall_write(void * pms);
int sgx_ocall_stat(void * pms);
int sgx_ocall_lstat(void * pms);
int sgx_ocall_fstat(void * pms);
int sgx_ocall_fsetnonblock(void * pms);
int sgx_ocall_fchmod(void * pms);
int sgx_ocall_fchdir(void * pms);
int sgx_ocall_fsync(void * pms);
int sgx_ocall_fdatasync(void * pms);
int sgx_ocall_ftruncate(void * pms);
int sgx_ocall_lseek64(void * pms);
int sgx_ocall_pread64(void * pms);
int sgx_ocall_pwrite64(void * pms);
int sgx_ocall_dup(void * pms);

// OS
int sgx_ocall_mkdir(void * pmd);
int sgx_ocall_rmdir(void * pmd);
int sgx_ocall_chdir(void * pmd);
int sgx_ocall_getcwd(void * pmd);
int sgx_ocall_link(void * pmd);
int sgx_ocall_unlink(void * pmd);
int sgx_ocall_truncate(void * pmd);
int sgx_ocall_rename(void * pmd);
int sgx_ocall_chmod(void * pmd);
int sgx_ocall_access(void * pmd);
int sgx_ocall_fcntl(void * pmd);
int sgx_ocall_getdents(void * pmd);

// Mem
int sgx_ocall_alloc_untrusted(void * pms);
int sgx_ocall_mmap_untrusted(void * pms);
int sgx_ocall_unmap_untrusted(void * pms);
int sgx_ocall_mprotect(void * pms);

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

// Misc 
int sgx_ocall_exit(void * pms);
int sgx_ocall_cpuid(void * pms);
int sgx_ocall_getpid(void * pms);
int sgx_ocall_fionread(void * pms);
int sgx_ocall_futex(void * pms);
int sgx_ocall_print_string(void * pms);
int sgx_ocall_debugp(void * pms);


#endif
