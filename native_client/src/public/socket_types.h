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

#endif
