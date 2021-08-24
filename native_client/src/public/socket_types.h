#ifndef _NATIVE_CLIENT_SRC_PUBLIC_SOCKET_TYPES_H_
#define _NATIVE_CLIENT_SRC_PUBLIC_SOCKET_TYPES_H_

#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/service_runtime/nacl_size_t.h"

#include "native_client/src/include/portability_sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __nacl_handle_defined
#define __nacl_handle_defined
typedef int NaClHandle;
#endif

// socket-related types

//# ifndef __socklen_t_defined
//typedef size_t socklen_t;
//# define __socklen_t_defined
//#endif

// sys/socket.h
#define PF_INET		2	/* IP protocol family.  */
#define AF_INET		PF_INET

// Note that other types are not available


#ifdef __cplusplus
}
#endif

#endif
