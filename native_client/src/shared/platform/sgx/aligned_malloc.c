/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "native_client/src/include/build_config.h"
#include "native_client/src/shared/platform/aligned_malloc.h"

#if NACL_SGX == 1

#include "native_client/src/trusted/stdlib/memheader.h"

void *NaClAlignedMalloc(size_t size, size_t alignment) {
  /*
   * Bionic in Android ICS (4.0) and earlier doesn't have
   * posix_memalign(), so we shall use memalign(), which
   * luckily, in Bionic, returns free()'able memory, although it is not
   * required to in general.
   * TODO(olonho): once/if we'll obsolete 4.0 support remove this #ifdef.
   */
  return memalign(alignment, size);
}

void NaClAlignedFree(void *block) {
  free(block);
}
#endif
