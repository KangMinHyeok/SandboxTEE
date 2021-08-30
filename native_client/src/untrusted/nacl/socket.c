/*
 * Wrapper for syscall.
 */

#include <errno.h>
#include <sys/types.h>

#include "native_client/src/public/imc_syscalls.h"
#include "native_client/src/untrusted/nacl/syscall_bindings_trampoline.h"

int socket(int domain, int type, int protocol) {
  int retval = NACL_SYSCALL(socket)(domain, type, protocol);
  if (retval < 0) {
    errno = -retval;
    return -1;
  }
  return retval;
}

int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen) {
  int retval = NACL_SYSCALL(bind)(sockfd, my_addr, addrlen);
  if (retval < 0) {
    errno = -retval;
    return -1;
  }
  return retval;
}

int listen(int sockfd, int backlog) {
  int retval = NACL_SYSCALL(listen)(sockfd, backlog);
  if (retval < 0) {
    errno = -retval;
    return -1;
  }
  return retval;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t addrlen) {
  int retval = NACL_SYSCALL(accept)(sockfd, addr, addrlen);
  if (retval < 0) {
    errno = -retval;
    return -1;
  }
  return retval;
}

// for client
int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen) {
  int retval = NACL_SYSCALL(connect)(sockfd, serv_addr, addrlen);
  if (retval < 0) {
    errno = -retval;
    return -1;
  }
  return retval;
}


