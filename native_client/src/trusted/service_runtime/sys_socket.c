
#include "native_client/src/trusted/service_runtime/sys_socket.h"

#include <string.h>

#include "native_client/src/trusted/desc/nacl_desc_socket.h"
#include "native_client/src/trusted/service_runtime/include/sys/errno.h"
#include "native_client/src/trusted/service_runtime/nacl_app_thread.h"
#include "native_client/src/trusted/service_runtime/nacl_copy.h"
#include "native_client/src/trusted/service_runtime/nacl_syscall_common.h"
#include "native_client/src/trusted/service_runtime/sel_ldr.h"


#include "native_client/src/public/socket_types.h"

int32_t NaClSysSocketOpen(struct NaClAppThread  *natp,
                          int32_t domain, int32_t type, int32_t protocol) {

  struct NaClApp       *nap = natp->nap;
  uint32_t             retval = (uint32_t) -NACL_ABI_EINVAL;
  struct NaClDesc *desc;

  NaClLog(3, "NaClSysSocketOpen(0x%08"NACL_PRIxPTR", "
          "0x%08"NACL_PRIx32", 0x%x, 0x%x)\n",
          (uintptr_t) natp, domain, type, protocol);

  // only tcp is available
  if (domain != AF_INET || type != SOCK_STREAM || protocol != IPPROTO_TCP) {
    retval = -NACL_ABI_ESOCKTNOSUPPORT;
    goto cleanup; 
  }
  
  desc = (struct NaClDesc *)NaClDescSocketDescMake();
  if (desc != NULL) {
    retval = NaClAppSetDescAvail(nap, desc);
  }
  
cleanup:
  return retval;

}

int32_t NaClSysSocketBind(struct NaClAppThread  *natp, 
                          int32_t sockfd, uint32_t myaddr, size_t addrlen) {
  struct NaClApp       *nap = natp->nap;
  struct NaClDesc *ndp;
  int retval = 0;
  uintptr_t sysaddr;

  ndp = NaClAppGetDesc(nap, sockfd);
  if (ndp == NULL){
    retval = -NACL_ABI_EBADR;
    goto cleanup;
  }

  sysaddr = NaClUserToSysAddrRange(nap, myaddr, addrlen);
  
  retval = (*((struct NaClDescVtbl const *) ndp->base.vtbl)->
                 Bind)(ndp, sysaddr, addrlen);
  if (retval != 0){
    retval = -NACL_ABI_EINVAL;
  }

cleanup:
  return retval;
}

int32_t NaClSysSocketListen(struct NaClAppThread  *natp, 
                          int32_t sockfd, int32_t backlog) {
  struct NaClApp       *nap = natp->nap;
  struct NaClDesc *ndp;
  int retval = 0;

  ndp = NaClAppGetDesc(nap, sockfd);
  if (ndp == NULL){
    retval = -NACL_ABI_EBADR;
    goto cleanup;
  }

  retval = (*((struct NaClDescVtbl const *) ndp->base.vtbl)->
                 Listen)(ndp, backlog);
 
cleanup:
  return retval;
}

int32_t NaClSysSocketAccept(struct NaClAppThread  *natp,
                    int32_t sockfd, uint32_t addr, size_t addrlen) {
  struct NaClApp       *nap = natp->nap;
  struct NaClDesc *ndp, *peer_ndp;
  int retval;
  uintptr_t sysaddr;

  ndp = NaClAppGetDesc(nap, sockfd);
  if (ndp == NULL){
    retval = -NACL_ABI_EBADR;
    goto cleanup;
  }

  sysaddr = NaClUserToSysAddrRange(nap, addr, addrlen);

  retval = (*((struct NaClDescVtbl const *) ndp->base.vtbl)->
                 Accept)(ndp, sysaddr, addrlen, &peer_ndp);
  
  if (retval == 0){
    retval = NaClAppSetDescAvail(nap, peer_ndp);
  }
  else {
    retval = -NACL_ABI_EINVAL;
    NaClDescUnref(ndp);
    goto cleanup;
  }

  NaClDescUnref(ndp);
  
cleanup:
  return retval;
}

int32_t NaClSysSocketConnect(struct NaClAppThread  *natp,
                  int32_t sockfd, uint32_t servaddr, size_t addrlen) {
  struct NaClApp       *nap = natp->nap;
  struct NaClDesc *ndp;
  int retval;
  uintptr_t sysaddr;

  ndp = NaClAppGetDesc(nap, sockfd);
  if (ndp == NULL){
    retval = -NACL_ABI_EBADR;
    goto cleanup;
  }

  sysaddr = NaClUserToSysAddrRange(nap, servaddr, addrlen);

  retval = (*((struct NaClDescVtbl const *) ndp->base.vtbl)->
                 Connect)(ndp, sysaddr, addrlen);
  

cleanup:
  NaClDescUnref(ndp);
  return retval;
}
