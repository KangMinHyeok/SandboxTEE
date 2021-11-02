/*
 * Copyright (c) 2008 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl Service Runtime.  I/O Descriptor / Handle abstraction.  Memory
 * mapping using descriptors.
 */

#ifndef NATIVE_CLIENT_SRC_TRUSTED_PLATFORM_LINUX_NACL_HOST_DESC_TYPES_H_
#define NATIVE_CLIENT_SRC_TRUSTED_PLATFORM_LINUX_NACL_HOST_DESC_TYPES_H_

#include "native_client/src/trusted/wolfssl/ssl.h"

// hmlee
struct DescCTX{
    WOLFSSL_CTX *ctx;
    uint8_t der_key[2048];
    uint8_t der_cert [8 * 1024];
    uint32_t der_key_len;
    uint32_t der_cert_len;

    char *redis_id;
    char *redis_pw;
    char *redis_id_len;
    char *redis_pw_len;
};

struct NaClHostDesc {
  int d;
  int flags;

  struct DescCTX *desc_ctx;

  /*
   * friend int NaClDescIoDescExternalizeSize(...);
   * friend int NaClDescIoDescExternalize(...);
   * friend int NaClDescIoInternalize(...);
   *
   * Translation: NaClDescIoDesc's externalization/internalization
   * interface functions are friend functions that look inside the
   * NaClHostDesc implementation.  Do not make changes here without
   * also looking there.
   */
};

#endif  /* NATIVE_CLIENT_SRC_TRUSTED_PLATFORM_LINUX_NACL_HOST_DESC_TYPES_H_ */
