/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef NATIVE_CLIENT_SRC_SHARED_PLATFORM_POSIX_NACL_FAST_MUTEX_H_
#define NATIVE_CLIENT_SRC_SHARED_PLATFORM_POSIX_NACL_FAST_MUTEX_H_

#if NACL_SGX == 1
// #include <sgx_thread.h>
#include "native_client/src/shared/platform/nacl_sync.h"
#include <stdint.h>
#else
#include <pthread.h>
#endif

#include "native_client/src/include/nacl_base.h"
/*
 * Structure definition for allocation purposes only.  We use
 * placement-new style construction, so object size must be exposed.
 */

/*
struct NaClFastMutex {
  struct NaClMutex mu;
};
*/
#endif  /* NATIVE_CLIENT_SRC_SHARED_PLATFORM_POSIX_NACL_FAST_MUTEX_H_ */
