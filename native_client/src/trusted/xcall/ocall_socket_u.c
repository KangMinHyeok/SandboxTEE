#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h> 
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <string.h>


#include "native_client/src/trusted/xcall/ocall_types.h"

#define DEFAULT_BACKLOG 2048

int sgx_ocall_socketpair(void * pms) {
	ms_ocall_socketpair_t * ms = (ms_ocall_socketpair_t *) pms;
	int ret = 0;

	ret = socketpair(ms->ms_domain, ms->ms_type|SOCK_CLOEXEC, ms->ms_protocol, ms->ms_sockfds);
	return ret;
}

int sgx_ocall_sock_listen(void * pms) {
	ms_ocall_sock_listen_t * ms = (ms_ocall_sock_listen_t *) pms;
	int ret = 0;
	int fd, ipv6only, reuseaddr;

	ret = socket(ms->ms_domain, ms->ms_type|SOCK_CLOEXEC, ms->ms_protocol);
	if (ret < 0)
		goto err;

	fd = ret;
	if (ms->ms_addr->sa_family == AF_INET6) {
		ipv6only = 1;
		ret = setsockopt(fd, SOL_IPV6, IPV6_V6ONLY, &ipv6only, sizeof(int));
		if (ret < 0)
			goto err;
	}

	reuseaddr = 1;
	ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int));
	if (ret < 0)
		goto err;

	ret = bind(fd, ms->ms_addr, ms->ms_addrlen);
	if (ret < 0)
		goto err_fd;

	if (ms->ms_type & SOCK_STREAM) {
		ret = listen(fd, DEFAULT_BACKLOG);
		if (ret < 0) 
			goto err_fd;
	}

	//ret = sock_getopt(fd, &ms->ms_sockopt);
	//if (ret < 0)
	//	goto eff_fd;

	return fd;

err_fd:
	close(fd);
err:
	return ret;
}

int sgx_ocall_sock_accept(void * pms) {
	int ret = 0;
	int fd;
	ms_ocall_sock_accept_t * ms = (ms_ocall_sock_accept_t *) pms;
	socklen_t addrlen = ms->ms_addrlen;

	ret = accept4(ms->ms_sockfd, ms->ms_addr, &addrlen, O_CLOEXEC);
	if (ret < 0)
		goto err;
	
	fd = ret;
	ms->ms_addrlen = addrlen;
	return fd;

err:
	return ret;
}

int sgx_ocall_sock_connect(void * pms) {
	ms_ocall_sock_connect_t * ms = (ms_ocall_sock_connect_t *) pms;
	int ret = 0;
	int fd, ipv6only;
	struct pollfd pfd = { .fd = 0, .events = POLLOUT, .revents = 0, };
	socklen_t addrlen;

	ret = socket(ms->ms_domain, ms->ms_type|SOCK_CLOEXEC, ms->ms_protocol);
	if (ret < 0)
		goto err;
	
	fd = ret;
	if (ms->ms_addr->sa_family == AF_INET6) {
		ipv6only = 1;
		setsockopt(fd, SOL_IPV6, IPV6_V6ONLY, &ipv6only, sizeof(int));
	}

	if (ms->ms_bind_addr && ms->ms_bind_addr->sa_family) {
		ret = bind(fd, ms->ms_bind_addr, ms->ms_bind_addrlen);
		if (ret < 0)
			goto err_fd;
	}

	ret = connect(fd, ms->ms_addr, ms->ms_addrlen);
	if ((ret < 0) && (-ret) == EINPROGRESS) {
		do {
			pfd.fd = fd;
			ret = ppoll(&pfd, 1, NULL, NULL);
		} while((ret < 0) && (-ret) == -EWOULDBLOCK);
	}

	if (ret < 0)
		goto err_fd;

	if (ms->ms_bind_addr && !ms->ms_bind_addr->sa_family) {
		ret = getsockname(fd, ms->ms_bind_addr, &addrlen);
		if (ret < 0)
			goto err_fd;
		ms->ms_bind_addrlen = addrlen;
	}

	return fd;

err_fd:
	close(fd);
err:
	return ret;
}

int sgx_ocall_sock_recv(void * pms)  {
	ms_ocall_sock_recv_t * ms = (ms_ocall_sock_recv_t *) pms; 
	int ret = 0; 
	struct sockaddr * addr = ms->ms_addr;
	socklen_t addrlen = ms->ms_addr ? ms->ms_addrlen : 0; 

	ret = recvfrom(ms->ms_sockfd, ms->ms_buf, ms->ms_count, 0, addr, addr ? &addrlen : NULL);
	if ((ret < 0) && addr)
		ms->ms_addrlen = addrlen;

	return ret; 
}

int sgx_ocall_sock_send(void * pms) {
	ms_ocall_sock_send_t * ms = (ms_ocall_sock_send_t *) pms;
	int ret = 0;
	const struct sockaddr * addr = ms->ms_addr;
	socklen_t addrlen = ms->ms_addr ? ms->ms_addrlen : 0;

	ret = sendto(ms->ms_sockfd, ms->ms_buf, ms->ms_count, MSG_NOSIGNAL,	addr, addrlen);

	return ret;
}

int sgx_ocall_sock_recv_fd(void * pms) {   
	ms_ocall_sock_recv_fd_t * ms = (ms_ocall_sock_recv_fd_t *) pms;
	int ret = 0;
	struct cmsghdr * chdr;
	struct msghdr hdr;
	struct iovec iov[1];

	char cbuf[sizeof(struct cmsghdr) + ms->ms_nfds * sizeof(int)];

	iov[0].iov_base = ms->ms_buf;
	iov[0].iov_len = ms->ms_count;

	memset(&hdr, 0, sizeof(struct msghdr));

	hdr.msg_iov = iov;
	hdr.msg_iovlen = 1;
	hdr.msg_control = cbuf;
	hdr.msg_controllen = sizeof(struct cmsghdr) + sizeof(int) * ms->ms_nfds;
	hdr.msg_flags = 0;

	ret = recvmsg(ms->ms_sockfd, &hdr, 0);

	if (ret < 0) {
		chdr = CMSG_FIRSTHDR(&hdr);
		if (chdr && chdr->cmsg_type == SCM_RIGHTS) {
			ms->ms_nfds = (chdr->cmsg_len - sizeof(struct cmsghdr)) / sizeof(int);
			memcpy(ms->ms_fds, CMSG_DATA(chdr), sizeof(int) * ms->ms_nfds);
		} else {
			ms->ms_nfds = 0;
		}

		return ret;
	}
	return ret;
}

int sgx_ocall_sock_send_fd(void * pms) {
	ms_ocall_sock_send_fd_t * ms = (ms_ocall_sock_send_fd_t *) pms;
	int ret = 0;
	struct msghdr hdr; 
	struct cmsghdr * chdr; 
	struct iovec iov[1]; 

	char cbuf[sizeof(struct cmsghdr) + ms->ms_nfds * sizeof(int)];

	iov[0].iov_base = (void *) ms->ms_buf;
	iov[0].iov_len = ms->ms_count;

	hdr.msg_name = NULL;
	hdr.msg_namelen = 0;
	hdr.msg_iov = iov;
	hdr.msg_iovlen = 1;
	hdr.msg_flags = 0;

	hdr.msg_control = cbuf; 
	hdr.msg_controllen = sizeof(struct cmsghdr) + sizeof(int) * ms->ms_nfds;

	chdr = CMSG_FIRSTHDR(&hdr); 
	chdr->cmsg_level = SOL_SOCKET; 
	chdr->cmsg_type = SCM_RIGHTS; 
	chdr->cmsg_len = CMSG_LEN(sizeof(int) * ms->ms_nfds);

	memcpy(CMSG_DATA(chdr), ms->ms_fds, sizeof(int) * ms->ms_nfds);

	hdr.msg_controllen = chdr->cmsg_len;

	ret = sendmsg(ms->ms_sockfd, &hdr, MSG_NOSIGNAL);

	return ret;
}

int sgx_ocall_sock_setopt(void * pms) {
	ms_ocall_sock_setopt_t * ms = (ms_ocall_sock_setopt_t *) pms;
	int ret = 0;
	
	ret = setsockopt(ms->ms_sockfd, ms->ms_level, ms->ms_optname,	ms->ms_optval, ms->ms_optlen);
	return ret;
}

int sgx_ocall_sock_shutdown(void * pms)
{
	ms_ocall_sock_shutdown_t * ms = (ms_ocall_sock_shutdown_t *) pms;
	int ret = 0;
	
	ret = shutdown(ms->ms_sockfd, ms->ms_how);
	return ret;
}


