#ifndef __ENCALVE_OCALLS__
#define __ENCLAVE_OCALLS__

#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "native_client/src/trusted/stdlib/filestruct.h"
#include "native_client/src/trusted/stdlib/stddef.h"


// Mem
int ocall_alloc_untrusted (uint64_t size, void ** mem);
int ocall_mmap_untrusted (int fd, uint64_t offset, uint64_t size, unsigned short prot, void ** mem);
int ocall_unmap_untrusted (const void * mem, uint64_t size);
int ocall_mprotect (void *addr, size_t len, int prot);

// File
int ocall_open (const char * pathname, int flags, unsigned short mode);
int ocall_close (int fd);
int ocall_read (int fd, void * buf, unsigned int count);
int ocall_write (int fd, const void * buf, unsigned int count);
int ocall_stat (const char *path, struct stat * buf);
int ocall_lstat (const char *path, struct stat * buf);
int ocall_fstat (int fd, struct stat * buf);
int ocall_fsetnonblock (int fd, int nonblocking);
int ocall_fchmod (int fd, unsigned short mode);
int ocall_fchdir (int fd);
int ocall_fsync (int fd);
int ocall_fdatasync (int fd);
int ocall_ftruncate (int fd, uint64_t length);
int ocall_pread64 (int fd, void * buf, size_t count, unsigned long pos);
int ocall_pwrite64 (int fd, const void * buf, size_t count, unsigned long offset);
// TODO: impl
int ocall_fflush(FILE *fp);
// TODO: impl
int ocall_ferror(FILE *fp);
// TODO: impl
long ocall_ftell(FILE *fp);
// TODO: impl
int ocall_fseek (FILE *fp, long offset, int whence);
// TODO: impl
FILE * ocall_fopen(const char *file, const char *mod);
// TODO: impl
int ocall_fclose(FILE *fp);
// TODO: impl
size_t ocall_fread(void *buf, size_t size, size_t count, register FILE *fp);
// TODO: impl
size_t ocall_fwrite(const void *prt, size_t size, size_t nmemb, register FILE *fp);
// TODO: impl
int ocall_flock(int fd, int operation);
// TODO: impl
FILE * ocall_fdopen(int fd, const char *mode);
// TODO: impl
int ocall_dup(int oldfd);
// TODO: impl
int ocall_lseek64(int fd, long int off, int whence);

// Dir 
int ocall_mkdir (const char * pathname, unsigned short mode);
int ocall_rmdir (const char * pathname);
int ocall_chdir (const char * pathname);
int ocall_getcwd (char * pathname, size_t size);
int ocall_link (const char * oldpath, const char * newpath);
int ocall_unlink (const char * pathname);
int ocall_truncate (const char * pathname, unsigned int size);
int	ocall_rename (const char * oldpath, const char * newpath);
int ocall_chmod(const char * path, uint64_t mode);
int ocall_access (const char * pathname, unsigned short mode);
int ocall_fcntl(int fd, int cmd, long args);
int ocall_getdents (unsigned int fd, struct linux_dirent64 * dirp, unsigned int size);

// Socket
int ocall_socketpair(int domain, int type, int protocol, int sockfds[2]);
int ocall_sock_listen (int domain, int type, int protocol, struct sockaddr * addr, unsigned int * addrlen);
int ocall_sock_accept(int sockfd, struct sockaddr * addr, unsigned int * addrlen);
int ocall_sock_connect (int domain, int type, int protocol, const struct sockaddr * addr, unsigned int addrlen,
		struct sockaddr * bind_addr, unsigned int * bind_addrlen);
int ocall_sock_recv (int sockfd, void * buf, unsigned int count, struct sockaddr * addr, unsigned int * addrlen);
int ocall_sock_send (int sockfd, const void * buf, unsigned int count, const struct sockaddr * addr, unsigned int addrlen);
int ocall_sock_recv_fd (int sockfd, void * buf, unsigned int count, unsigned int * fds, unsigned int * nfds);
int ocall_sock_send_fd (int sockfd, const void * buf, unsigned int count, const unsigned int * fds, unsigned int nfds);
int ocall_sock_setopt (int sockfd, int level, int optname, const void * optval, unsigned int optlen);
int ocall_sock_shutdown (int sockfd, int how);

// Time
// TODO: impl
int ocall_gettime (struct timeval *tv);
int ocall_gettimeofday (struct timeval *tv);
int ocall_clock_gettime (int clk_id, struct timespec *tp);
int ocall_sleep(unsigned long * microsec);
int ocall_nsleep(struct timespec *rqtp, struct timespec *rmtp);

// Misc 
int ocall_cpuid (unsigned int leaf, unsigned int subleaf, unsigned int values[4]);
int ocall_getpid ();
int ocall_fionread (int fd);
int ocall_futex (int * futex, int op, int val, const uint64_t * timeout, int * uaddr2, int val3);
int ocall_print_string (const char * str, unsigned int length);
// TODO: impl
int ocall_fprint_string (FILE *fp, const char * str, unsigned int length);
int ocall_debugp (int val);
// TODO: impl
void ocall_exit(int status);


#endif
