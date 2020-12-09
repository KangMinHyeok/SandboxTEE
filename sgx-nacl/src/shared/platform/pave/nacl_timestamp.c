/*
 * Copyright 2008 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl Service Runtime logging code.
 */

#include <stdint.h>
#include <stddef.h>

#include "native_client/src/include/build_config.h"
#include "native_client/src/shared/platform/nacl_timestamp.h"

#if NACL_PAVE
#include "native_client/src/trusted/sgxlib/enclave_ocalls.h"
#include "native_client/src/trusted/stdlib/api.h"
#include "native_client/src/shared/platform/pave/linux_types.h"
#endif

char  *NaClTimeStampString(char   *buffer,
                           size_t buffer_size) {
//  struct timeval  tv;
//  struct tm       bdt;  /* broken down time */

//  if (-1 == gettimeofday(&tv, (struct timezone *) NULL)) {
    snprintf(buffer, buffer_size, "-NA-");
    return buffer;
/*  }
  (void) localtime_r(&tv.tv_sec, &bdt);
  // * suseconds_t holds at most 10**6 < 16**6 = (2**4)**6 = 2**24 < 2**32
  snprintf(buffer, buffer_size, "%02d:%02d:%02d.%06d",
           bdt.tm_hour, bdt.tm_min, bdt.tm_sec, (int) tv.tv_usec);
  return buffer; */
}
