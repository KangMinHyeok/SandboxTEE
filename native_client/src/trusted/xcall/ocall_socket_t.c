#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/ecall_types.h"

int ocall_socketpair(int domain, int type, int protocol, int sockfds[2]) {
	int retval = 0;
	ms_ocall_socketpair_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_domain = domain;
	ms->ms_type = type;
	ms->ms_protocol = protocol;

	retval = sgx_ocall(OCALL_SOCKETPAIR, ms);
	if (!retval) {
		sockfds[0] = ms->ms_sockfds[0];
		sockfds[1] = ms->ms_sockfds[1];
	}

	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sock_listen (int domain, int type, int protocol,
		struct sockaddr * addr, unsigned int * addrlen
		) {

	int retval = 0; 
	unsigned int bind_len = *addrlen;
	ms_ocall_sock_listen_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_domain = domain;
	ms->ms_type = type;
	ms->ms_protocol = protocol;
	ms->ms_addr = sgx_copy_to_ustack(addr, bind_len);
	ms->ms_addrlen = bind_len;

	retval = sgx_ocall(OCALL_SOCK_LISTEN, ms); 
	if (retval >= 0) { 
		if (addrlen && (
					sgx_is_completely_within_enclave(ms->ms_addr, bind_len) ||
					ms->ms_addrlen > bind_len)) {
			sgx_reset_ustack(old_ustack);
			return -1;
		}

		if (addr) {
			sgx_copy_to_enclave(addr, *addrlen, ms->ms_addr, ms->ms_addrlen);
			//COPY_FROM_USER(addr, ms->ms_addr, ms->ms_addrlen);
			*addrlen = ms->ms_addrlen;
		}    
		//if (sockopt)
		//	*sockopt = ms->ms_sockopt;
	}    
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sock_accept(int sockfd, struct sockaddr * addr, unsigned int * addrlen) {
	int retval = 0;
	unsigned int len = addrlen ? *addrlen : 0;
	ms_ocall_sock_accept_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_sockfd = sockfd;
	ms->ms_addr = sgx_copy_to_ustack(addr, len);
	ms->ms_addrlen = len;

	retval = sgx_ocall(OCALL_SOCK_ACCEPT, ms);
	if (retval >= 0) {
		if (len && (sgx_is_completely_within_enclave(ms->ms_addr, len) || ms->ms_addrlen > len)) {
			sgx_reset_ustack(old_ustack);
			return -1;
		}
		if (addr) {
			sgx_copy_to_enclave(addr, *addrlen, ms->ms_addr, ms->ms_addrlen);
			*addrlen = ms->ms_addrlen;
		}
	}
	sgx_reset_ustack(old_ustack);

	return retval;
}

int ocall_sock_connect (int domain, int type, int protocol, struct sockaddr * addr,	unsigned int addrlen,
		struct sockaddr * bind_addr, unsigned int * bind_addrlen) {   
	int retval = 0;
	unsigned int bind_len = bind_addrlen ? *bind_addrlen : 0;
	ms_ocall_sock_connect_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_domain = domain;
	ms->ms_type = type;
	ms->ms_protocol = protocol;
	ms->ms_addr = sgx_copy_to_ustack(addr, addrlen);
	ms->ms_addrlen = addrlen;
	ms->ms_bind_addr = bind_addr ? sgx_copy_to_ustack(bind_addr, bind_len) : NULL;
	ms->ms_bind_addrlen = bind_len;

	retval = sgx_ocall(OCALL_SOCK_CONNECT, ms);
	if (retval >= 0) {
		if (bind_len && (
					sgx_is_completely_within_enclave (ms->ms_bind_addr, bind_len) ||
					ms->ms_bind_addrlen > bind_len)) {
			sgx_reset_ustack(old_ustack);
			return -1;
		}

		if (bind_addr) {
			sgx_copy_to_enclave(bind_addr, *bind_addrlen, ms->ms_bind_addr, ms->ms_bind_addrlen);
			*bind_addrlen = ms->ms_bind_addrlen;
		}
	}
	sgx_reset_ustack(old_ustack);
	
	return retval;
}


int ocall_sock_recv (int sockfd, void * buf, unsigned int count, struct sockaddr * addr, unsigned int * addrlen) {
	int retval = 0;
	unsigned int len = addrlen ? *addrlen : 0;
	ms_ocall_sock_recv_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_sockfd = sockfd;
	ms->ms_buf = sgx_alloc_on_ustack(count);
	ms->ms_count = count;
	ms->ms_addr = addr ? sgx_alloc_on_ustack(len) : NULL;
	ms->ms_addrlen = len;

	retval = sgx_ocall(OCALL_SOCK_RECV, ms);
	if (retval >= 0) {
		if (len && (sgx_is_completely_within_enclave(ms->ms_addr, len) ||
					ms->ms_addrlen > len)) {
			sgx_reset_ustack(old_ustack);
			return -1;
		}

		sgx_copy_to_enclave(buf, count, ms->ms_buf, retval);
		sgx_copy_to_enclave(addr, *addrlen, ms->ms_addr, ms->ms_addrlen);
		if (addrlen)
			*addrlen = ms->ms_addrlen;
	}
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sock_send (int sockfd, void * buf, unsigned int count, struct sockaddr * addr, unsigned int addrlen) {
	int retval = 0;
	ms_ocall_sock_send_t * ms;
	void* old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_sockfd = sockfd;
	ms->ms_buf = sgx_copy_to_ustack(buf, count);
	ms->ms_count = count;
	ms->ms_addr = addr ? sgx_copy_to_ustack(addr, addrlen) : NULL;
	ms->ms_addrlen = addrlen;

	retval = sgx_ocall(OCALL_SOCK_SEND, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sock_recv_fd (int sockfd, void * buf, unsigned int count,	unsigned int * fds, unsigned int * nfds) {
	int retval = 0;
	ms_ocall_sock_recv_fd_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_sockfd = sockfd;
	ms->ms_buf = sgx_alloc_on_ustack(count);
	ms->ms_count = count;
	ms->ms_fds = fds ? sgx_alloc_on_ustack(sizeof(int) * (*nfds)) : NULL;
	ms->ms_nfds = *nfds;

	retval = sgx_ocall(OCALL_SOCK_RECV_FD, ms);
	if (retval >= 0) {
		if (sgx_is_completely_within_enclave(ms->ms_fds, sizeof(int) * (*nfds)) || ms->ms_nfds > (*nfds)) {
			sgx_reset_ustack(old_ustack);
			return -1;
		}

		sgx_copy_to_enclave(buf, count, ms->ms_buf, retval);
		sgx_copy_to_enclave(fds, *nfds, ms->ms_fds, sizeof(int) * ms->ms_nfds);
		*nfds = ms->ms_nfds;
	}
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sock_send_fd (int sockfd, void * buf, unsigned int count,	unsigned int * fds, unsigned int nfds) {
	int retval = 0;
	ms_ocall_sock_send_fd_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_sockfd = sockfd;
	ms->ms_buf = sgx_copy_to_ustack(buf, count);
	ms->ms_count = count;
	ms->ms_fds = fds ? sgx_copy_to_ustack(fds, sizeof(int) * nfds) : NULL;
	ms->ms_nfds = nfds;

	retval = sgx_ocall(OCALL_SOCK_SEND_FD, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sock_setopt (int sockfd, int level, int optname, void * optval, unsigned int optlen) {
	int retval = 0;
	ms_ocall_sock_setopt_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_sockfd = sockfd;
	ms->ms_level = level;
	ms->ms_optname = optname;
	ms->ms_optval = sgx_copy_to_ustack(optval, optlen);
	ms->ms_optlen = optlen;

	retval = sgx_ocall(OCALL_SOCK_SETOPT, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sock_shutdown (int sockfd, int how) {
	int retval = 0;
	ms_ocall_sock_shutdown_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_sockfd = sockfd;
	ms->ms_how = how;

	retval = sgx_ocall(OCALL_SOCK_SHUTDOWN, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

