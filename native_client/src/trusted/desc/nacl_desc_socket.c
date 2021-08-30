/*
 * NaCl Service Runtime.  Socket Descriptor / Handle abstraction.  Memory
 * mapping using descriptors.
 */

#include "native_client/src/include/build_config.h"
#include "native_client/src/include/portability.h"

#if NACL_SGX == 1

#include <string.h>

#include "native_client/src/include/nacl_macros.h"
#include "native_client/src/shared/imc/nacl_imc_c.h"
#include "native_client/src/trusted/desc/nacl_desc_base.h"
#include "native_client/src/trusted/desc/nacl_desc_effector.h"
#include "native_client/src/trusted/desc/nacl_desc_socket.h"

#include "native_client/src/shared/platform/nacl_find_addrsp.h"
#include "native_client/src/shared/platform/nacl_host_desc.h"
#include "native_client/src/shared/platform/nacl_log.h"

#include "native_client/src/trusted/service_runtime/include/bits/mman.h"
#include "native_client/src/trusted/service_runtime/include/sys/errno.h"
#include "native_client/src/trusted/service_runtime/include/sys/fcntl.h"
#include "native_client/src/trusted/service_runtime/internal_errno.h"
#include "native_client/src/trusted/service_runtime/nacl_config.h"

#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/public/socket_types.h"

static struct NaClDescVtbl const kNaClDescSocketDescVtbl;

static int NaClSetHostSockDesc(struct NaClHostDesc *hd, int sock_desc) {
  hd->d = sock_desc;
  // is it required?
  // hd->flags = NACL_ABI_O_RDWR;
  return 0;
}


int NaClDescSocketDescCtor(struct NaClDescSocketDesc  *self,
                       struct NaClHostDesc    *hd) {
  struct NaClDesc *basep = (struct NaClDesc *) self;

  if (!NaClDescCtor(basep)) {
    return 0;
  }

  basep->base.vtbl = (struct NaClRefCountVtbl const *) &kNaClDescSocketDescVtbl;
  self->hd = hd;

  NaClSetHostSockDesc(hd, 0);
  
  return 1;
}

static void NaClDescSocketDescDtor(struct NaClRefCount *vself) {
  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;

  NaClLog(4, "NaClDescSocketDescDtor(0x%08"NACL_PRIxPTR").\n",
          (uintptr_t) vself);
  if (0 != NaClHostDescClose(self->hd)) {
    NaClLog(LOG_FATAL, "NaClDescSocketDescDtor: NaClHostDescClose failed\n");
  }
  free(self->hd);
  self->hd = NULL;
  vself->vtbl = (struct NaClRefCountVtbl const *) &kNaClDescVtbl;
  (*vself->vtbl->Dtor)(vself);
}

struct NaClDescSocketDesc *NaClDescSocketDescMake() {
  struct NaClDescSocketDesc *sdp = NULL;
  struct NaClHostDesc *hd = NULL;

  hd = malloc(sizeof *hd);
  if (hd == NULL){
    NaClLog(LOG_FATAL, "%s (%d): failed", __func__, __LINE__);
    goto cleanup;
  }

  // dummy
  /*
  if (NaClHostSocketOpen(hd) != 0){ 
    NaClLog(LOG_FATAL, "%s (%d): failed", __func__, __LINE__);
    goto cleanup;
  }
  */

  sdp = malloc(sizeof *sdp);
  if (NULL == sdp) {
    NaClLog(LOG_FATAL, "%s (%d): failed", __func__, __LINE__);
    goto cleanup;
  }

  if (!NaClDescSocketDescCtor(sdp, hd)) {
    NaClLog(LOG_FATAL, "%s (%d): failed", __func__, __LINE__);
    free(sdp);
    free(hd);
    goto cleanup;
  }

cleanup:
  return sdp;
}

ssize_t NaClDescSocketDescRead(struct NaClDesc *vself, void *buf, size_t len) {
  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
  return ocall_read(self->hd->d, buf, len);
}

ssize_t NaClDescSocketDescWrite(struct NaClDesc         *vself,
                                    void const              *buf,
                                    size_t                  len) {

  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
  return ocall_write(self->hd->d, buf, len);
}


int32_t NaClDescSocketBind(struct NaClDesc *vself, uintptr_t addr, int addrlen){
  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
  self->myaddr = (struct sockaddr *) addr;
  self->myaddrlen = addrlen;
  return 0;
}


int32_t NaClDescSocketListen(struct NaClDesc *vself, int backlog) {
  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
  int retval;

  if (self->myaddr == NULL || self->myaddrlen == 0){
    retval =  -NACL_ABI_EINVAL;
  }
  retval = ocall_sock_listen(AF_INET, SOCK_STREAM, IPPROTO_TCP, self->myaddr, &self->myaddrlen);

  // retval == socket desc for myself
  if (retval > 0) {
    NaClSetHostSockDesc(self->hd, retval);
  }
  
  return retval;
}


int NaClDescSocketAccept(struct NaClDesc *vself, uintptr_t addr, size_t addrlen, struct NaClDesc **peer) {
  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
  int retval;

  retval = ocall_sock_accept(self->hd->d, (struct sockaddr *) addr, (unsigned int *) &addrlen);

  // retval == client descriptor
  if (retval < 0) {
    retval = -NACL_ABI_EINVAL;
    goto cleanup;
  }

  // set peer socket 
  *peer = (struct NaClDesc *) NaClDescSocketDescMake();
  NaClSetHostSockDesc(((struct NaClDescSocketDesc *) *peer)->hd, retval);
  retval = 0;
  
cleanup:
  return retval;
}


int32_t NaClDescSocketConnect(struct NaClDesc *vself, uintptr_t addr, int addrlen){
  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
  int retval;

  retval = ocall_sock_connect(AF_INET, SOCK_STREAM, IPPROTO_TCP, (struct sockaddr *)addr, addrlen, NULL, NULL);

  if (retval < 0) {
    retval = -NACL_ABI_EINVAL;
    goto cleanup;
  }

  // retval = fd for mine?
  NaClSetHostSockDesc(self->hd, retval);
  retval = 0;
  
cleanup:
  return retval;
}


static struct NaClDescVtbl const kNaClDescSocketDescVtbl = {
  {
    NaClDescSocketDescDtor,
  },
  // bind & listen is not implemented as vtbl
  NaClDescMapNotImplemented,
  NaClDescSocketDescRead, // here
  NaClDescSocketDescWrite, // here
  NaClDescSeekNotImplemented,
  NaClDescPReadNotImplemented,
  NaClDescPWriteNotImplemented,
  NaClDescFstatNotImplemented,
  NaClDescFchdirNotImplemented,
  NaClDescFchmodNotImplemented,
  NaClDescFsyncNotImplemented,
  NaClDescFdatasyncNotImplemented,
  NaClDescFtruncateNotImplemented,
  NaClDescGetdentsNotImplemented,
  NaClDescExternalizeSizeNotImplemented,
  NaClDescExternalizeNotImplemented,
  NaClDescLockNotImplemented,
  NaClDescTryLockNotImplemented,
  NaClDescUnlockNotImplemented,
  NaClDescWaitNotImplemented,
  NaClDescTimedWaitAbsNotImplemented,
  NaClDescSignalNotImplemented,
  NaClDescBroadcastNotImplemented,
  NaClDescSendMsgNotImplemented,
  NaClDescRecvMsgNotImplemented,
  NaClDescLowLevelSendMsgNotImplemented,
  NaClDescLowLevelRecvMsgNotImplemented,
  NaClDescConnectAddrNotImplemented,
  NaClDescAcceptConnNotImplemented,
  NaClDescPostNotImplemented,
  NaClDescSemWaitNotImplemented,
  NaClDescGetValueNotImplemented,
  NaClDescSetMetadata,
  NaClDescGetMetadata,
  NaClDescSetFlags,
  NaClDescGetFlags,
  NaClDescIsattyNotImplemented,
  NaClDescSocketBind, // here
  NaClDescSocketListen, // here
  NaClDescSocketAccept, // here
  NaClDescSocketConnect, // here
  NACL_DESC_HOST_SOCKET,
};

#endif
