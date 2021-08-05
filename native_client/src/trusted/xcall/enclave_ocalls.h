#ifndef __ENCALVE_OCALLS__
#define __ENCLAVE_OCALLS__

#include <time.h>

// File
int ocall_open (char * pathname, int flags, unsigned short mode);
int ocall_close (int fd);
int ocall_read (int fd, void * buf, unsigned int count);
int ocall_write (int fd, void * buf, unsigned int count);


// Mem
int ocall_alloc_untrusted (uint64_t size, void ** mem);
int ocall_mmap_untrusted (int fd, uint64_t offset, uint64_t size, unsigned short prot, void ** mem);
int ocall_unmap_untrusted (void * mem, uint64_t size);
int ocall_mprotect (void *addr, size_t len, int prot);

// Socket
int ocall_socketpair(int domain, int type, int protocol, int sockfds[2]);
int ocall_sock_listen (int domain, int type, int protocol, struct sockaddr * addr, unsigned int * addrlen);
int ocall_sock_accept(int sockfd, struct sockaddr * addr, unsigned int * addrlen);
int ocall_sock_connect (int domain, int type, int protocol, struct sockaddr * addr, unsigned int addrlen,
		struct sockaddr * bind_addr, unsigned int * bind_addrlen);
int ocall_sock_recv (int sockfd, void * buf, unsigned int count, struct sockaddr * addr, unsigned int * addrlen);
int ocall_sock_send (int sockfd, void * buf, unsigned int count, struct sockaddr * addr, unsigned int addrlen);
int ocall_sock_recv_fd (int sockfd, void * buf, unsigned int count, unsigned int * fds, unsigned int * nfds);
int ocall_sock_send_fd (int sockfd, void * buf, unsigned int count, unsigned int * fds, unsigned int nfds);
int ocall_sock_setopt (int sockfd, int level, int optname, void * optval, unsigned int optlen);
int ocall_sock_shutdown (int sockfd, int how);

// Time
int ocall_gettimeofday (struct timeval *tv);
int ocall_clock_gettime (int clk_id, struct timespec *tp);
int ocall_sleep(unsigned long * microsec);
int ocall_nsleep(struct timespec *rqtp, struct timespec *rmtp);

// Misc 
int ocall_cpuid(void * pms);
int ocall_getpid(void * pms);
int ocall_fionread(void * pms);
int ocall_futex(void * pms);
int ocall_print_string(void * pms);
int ocall_debugp(void * pms);


// Time
//int ocall_gettimeofday (struct timeval *tv);
//int ocall_clock_gettime (int clk_id, struct timespec *tp);


//int ocall_debugp (int val);
//int ocall_print_string(char * buf, int idx);




#endif
