/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl service runtime.  NaClDescSocketDesc subclass of NaClDesc.
 */
#ifndef NATIVE_CLIENT_SRC_TRUSTED_DESC_NACL_DESC_SOCKET_H_
#define NATIVE_CLIENT_SRC_TRUSTED_DESC_NACL_DESC_SOCKET_H_

#include "native_client/src/include/nacl_base.h"
#include "native_client/src/include/portability.h"

#include "native_client/src/public/nacl_desc.h"
#include "native_client/src/trusted/desc/nacl_desc_base.h"
#include "native_client/src/trusted/wolfssl/handshake.h"

EXTERN_C_BEGIN

struct NaClDescEffector;
struct NaClDescXferState;
struct NaClMessageHeader;

/*
 * Socket descriptors
 */

struct NaClDescSocketDesc {
  struct NaClDesc           base NACL_IS_REFCOUNT_SUBCLASS;
  struct NaClHostDesc       *hd;

  // race prevention is needed?
  

  // for ssl
	WOLFSSL_CTX* ctx;
	WOLFSSL *ssl;


  // in server mode;
  struct sockaddr *myaddr;
  socklen_t myaddrlen;

};


int NaClDescSocketDescCtor(struct NaClDescSocketDesc  *self,
                       struct NaClHostDesc    *hd) NACL_WUR;

struct NaClDescSocketDesc *NaClDescSocketDescMake();

EXTERN_C_END

#endif  // NATIVE_CLIENT_SRC_TRUSTED_DESC_NACL_DESC_SOCKET_H_
