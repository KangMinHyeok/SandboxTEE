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

#include "native_client/src/trusted/wolfssl/handshake.h"

#define MQTT_TLS 0

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

static int NaClDescSocketMakeSSLContext(struct NaClDescSocketDesc *sdp) {

  int ret = 0;
	WOLFSSL_CTX* ctx;
	WOLFSSL_METHOD* method;

  method = wolfTLSv1_2_client_method();
	if (method == NULL) {
		goto cleanup;
	}

  sdp->ctx = wolfSSL_CTX_new(method);
	if (sdp->ctx == NULL) {
		printf("ERROR: wolfSSL_CTX_new failed\n");
		goto cleanup;
	}

  ctx = sdp->ctx;
  
	if ((ret = wolfSSL_CTX_load_verify_buffer(ctx, ca_cert, sizeof(ca_cert), SSL_FILETYPE_ASN1)) != SSL_SUCCESS) {
		printf("ERROR: failed to load cert, please check the file.\n");
		goto cleanall;
	}

	wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, cert_verify_callback);

	if ((sdp->ssl = wolfSSL_new(ctx)) == NULL) {
		printf("ERROR: failed to create WOLFSSL object\n");
		ret = -1;
		goto cleanall;
	}

  return 0;

cleanall:
	wolfSSL_CTX_free(sdp->ctx);
	wolfSSL_Cleanup();
cleanup:
  return -1;
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
    goto cleanall;
  }

#if MQTT_TLS
  if (!NaClDescSocketMakeSSLContext(sdp)){
    NaClLog(LOG_FATAL, "%s (%d): failed", __func__, __LINE__);
    goto cleanall;
  }
#endif

cleanall:
  free(sdp);
  free(hd);

  sdp = NULL;
  hd = NULL;

cleanup:
  return sdp;
}

ssize_t NaClDescSocketDescRead(struct NaClDesc *vself, void *buf, size_t len) {
  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
#if MQTT_TLS
  return wolfSSL_read(self->ssl, buf, len);
#else
  return ocall_read(self->hd->d, buf, len);
#endif
}

ssize_t NaClDescSocketDescWrite(struct NaClDesc         *vself,
                                    void const              *buf,
                                    size_t                  len) {

  struct NaClDescSocketDesc *self = (struct NaClDescSocketDesc *) vself;
#if MQTT_TLS
  return wolfSSL_write(self->ssl, buf, len);
#else
  return ocall_write(self->hd->d, buf, len);
#endif
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

#if MQTT_TLS
  retval = wolfSSL_set_fd(self->ssl, self->hd->d);
  if (retval != WOLFSSL_SUCCESS) {
    printf("error: failed to set fdn");
    goto cleanup;
  }

  retval = wolfSSL_connect(self->ssl);
  if (retval != SSL_SUCCESS) {
    printf("error: failed to connect to wolfSSL\n");
    retval = -NACL_ABI_EINVAL;
  }

#endif
  
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
