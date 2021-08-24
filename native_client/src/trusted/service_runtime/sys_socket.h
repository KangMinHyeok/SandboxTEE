#ifndef NATIVE_CLIENT_SRC_TRUSTED_SERVICE_RUNTIME_SYS_SOCKET_H_
#define NATIVE_CLIENT_SRC_TRUSTED_SERVICE_RUNTIME_SYS_SOCKET_H_ 1

#include "native_client/src/include/nacl_base.h"
#include "native_client/src/include/portability.h"

#if NACL_SGX == 1

EXTERN_C_BEGIN

struct NaClAppThread;


int32_t NaClSysSocketOpen(struct NaClAppThread  *natp,
                          int32_t domain, int32_t type, int32_t protocol) ;


int32_t NaClSysSocketBind(struct NaClAppThread  *natp, 
                          int32_t sockfd, uint32_t myaddr, size_t addrlen) ;


int32_t NaClSysSocketListen(struct NaClAppThread  *natp, 
                          int32_t sockfd, int32_t backlog) ;


int32_t NaClSysSocketAccept(struct NaClAppThread  *natp,
                    int32_t sockfd, uint32_t addr, size_t addrlen);


int32_t NaClSysSocketConnect(struct NaClAppThread  *natp,
                  int32_t sockfd, uint32_t servaddr, size_t addrlen) ;


EXTERN_C_END

#endif

#endif
