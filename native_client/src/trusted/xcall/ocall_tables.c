
#include "native_client/src/trusted/xcall/ocall_types.h"

typedef int (*sgx_ocall_fn_t)(void*);


sgx_ocall_fn_t ocall_table[OCALL_NR] = {

	
	[OCALL_FUTEX] = sgx_ocall_futex,

	[OCALL_SOCKETPAIR] = sgx_ocall_socketpair,
	[OCALL_SOCK_LISTEN] = sgx_ocall_sock_listen,
	[OCALL_SOCK_ACCEPT] = sgx_ocall_sock_accept,
	[OCALL_SOCK_CONNECT] = sgx_ocall_sock_connect,
	[OCALL_SOCK_RECV] = sgx_ocall_sock_recv,
	[OCALL_SOCK_SEND] = sgx_ocall_sock_send,
	[OCALL_SOCK_RECV_FD] = sgx_ocall_sock_recv_fd,
	[OCALL_SOCK_SEND_FD] = sgx_ocall_sock_send_fd,
	[OCALL_SOCK_SETOPT] = sgx_ocall_sock_setopt,
	[OCALL_SOCK_SHUTDOWN] = sgx_ocall_sock_shutdown,

	[OCALL_GETTIMEOFDAY] = sgx_ocall_gettimeofday,
	[OCALL_CLOCK_GETTIME] = sgx_ocall_clock_gettime,
	[OCALL_SLEEP] = sgx_ocall_sleep,
	[OCALL_NSLEEP] = sgx_ocall_nsleep,
	
	[OCALL_DEBUGP] =  sgx_ocall_debugp,
};


