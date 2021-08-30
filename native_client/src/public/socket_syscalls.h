#ifndef _NATIVE_CLIENT_SRC_PUBLIC_SOCKET_SYSCALLS_H_
#define _NATIVE_CLIENT_SRC_PUBLIC_SOCKET_SYSCALLS_H_

#ifndef _NATIVE_CLIENT_SRC_PUBLIC_SOCKET_TYPES_H_
#error "include socket_types.h first"
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern int socket(int domain, int type, int protocol) ;

extern int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen) ;

extern int listen(int sockfd, int backlog) ;

extern int accept(int sockfd, struct sockaddr *addr, socklen_t addrlen) ;

extern int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen) ;

#ifdef __cplusplus
}
#endif

#endif
