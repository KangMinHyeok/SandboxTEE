#ifndef _NATIVE_CLIENT_SRC_PUBLIC_SOCKET_TYPES_H_
#define _NATIVE_CLIENT_SRC_PUBLIC_SOCKET_TYPES_H_

#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/service_runtime/nacl_size_t.h"

#ifndef __native_client__
#include "native_client/src/include/portability_sockets.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __nacl_handle_defined
#define __nacl_handle_defined
typedef int NaClHandle;
#endif

// socket-related types

# ifndef __socklen_t_defined
typedef size_t socklen_t;
# define __socklen_t_defined
#endif

// sys/socket.h
#define PF_INET		2	/* IP protocol family.  */
#define AF_INET		PF_INET

#ifdef __native_client__
// bits/sockaddr.h and bits/socket.h
// it is architecture-dependent

typedef unsigned short int sa_family_t;

#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

struct sockaddr {
    __SOCKADDR_COMMON (sa_);	/* Common data: address family and length.  */
    char sa_data[14];		/* Address data.  */
};

#endif

// Note that other types are not available


#ifdef __cplusplus
}
#endif

#ifndef _SYS_SOCKET_H
enum __socket_type
{
  SOCK_STREAM = 1,                /* Sequenced, reliable, connection-based
                                   byte streams.  */
#define SOCK_STREAM SOCK_STREAM
  SOCK_DGRAM = 2,                /* Connectionless, unreliable datagrams
                                   of fixed maximum length.  */
#define SOCK_DGRAM SOCK_DGRAM
  SOCK_RAW = 3,                        /* Raw protocol interface.  */
#define SOCK_RAW SOCK_RAW
  SOCK_RDM = 4,                        /* Reliably-delivered messages.  */
#define SOCK_RDM SOCK_RDM
  SOCK_SEQPACKET = 5,                /* Sequenced, reliable, connection-based,
                                   datagrams of fixed maximum length.  */
#define SOCK_SEQPACKET SOCK_SEQPACKET
  SOCK_DCCP = 6,                /* Datagram Congestion Control Protocol.  */
#define SOCK_DCCP SOCK_DCCP
  SOCK_PACKET = 10,                /* Linux specific way of getting packets
                                   at the dev level.  For writing rarp and
                                   other similar things on the user level. */
#define SOCK_PACKET SOCK_PACKET

  /* Flags to be ORed into the type parameter of socket and socketpair and
     used for the flags parameter of paccept.  */

  SOCK_CLOEXEC = 02000000,        /* Atomically set close-on-exec flag for the
                                   new descriptor(s).  */
#define SOCK_CLOEXEC SOCK_CLOEXEC
  SOCK_NONBLOCK = 04000                /* Atomically mark descriptor(s) as
                                   non-blocking.  */
#define SOCK_NONBLOCK SOCK_NONBLOCK
};

struct in_addr {     
	unsigned long s_addr; 
};

struct sockaddr_in { 
    short sin_family;  
    unsigned short sin_port; 
    struct in_addr sin_addr; 
    char sin_zero[8]; 
};  


struct addrinfo { 
    int ai_flags; 
    int ai_family; 
    int ai_socktype; 
    int ai_protocol; 
    socklen_t ai_addrlen; 
    char *ai_canonname; 
    struct sockaddr *ai_addr; 
    struct addrinfo *ai_next; 
};
#endif 

#ifndef IPPROTO_IP
#define	IPPROTO_IP		0		/* dummy for IP */
#endif

#ifndef IPPROTO_ICMP
#define	IPPROTO_ICMP		1		/* control message protocol */
#endif

#ifndef IPPROTO_TCP
#define	IPPROTO_TCP		6		/* tcp */
#endif

#ifndef IPPROTO_UDP
#define	IPPROTO_UDP		17		/* user datagram protocol */
#endif

#ifndef INADDR_ANY
#define	INADDR_ANY		(u_int32_t)0x00000000
#endif

#ifndef INADDR_BROADCAST
#define	INADDR_BROADCAST	(u_int32_t)0xffffffff	/* must be masked */
#endif

#ifndef AF_INET6
#define AF_INET6 10
#endif

#ifndef AF_INET
#define AF_INET 2
#endif

#endif
