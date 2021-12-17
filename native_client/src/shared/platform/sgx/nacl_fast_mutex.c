/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "native_client/src/shared/platform/nacl_check.h"
#include "native_client/src/shared/platform/nacl_host_desc.h"
#include "native_client/src/shared/platform/sgx/nacl_fast_mutex.h"

#include "native_client/src/trusted/xcall/enclave_ocalls.h"

int NaClFastMutexCtor(struct NaClFastMutex *flp) {
  return NaClMutexCtor(&flp->mu); 
}

void NaClFastMutexDtor(struct NaClFastMutex *flp) {
  NaClMutexDtor(&flp->mu); 
}

void NaClFastMutexLock(struct NaClFastMutex *flp) {
  int ret = NaClMutexLock(&flp->mu); 
  UNREFERENCED_PARAMETER(ret);
}

int NaClFastMutexTryLock(struct NaClFastMutex *flp) {
  int ret = NaClMutexTryLock(&flp->mu); 
  return ret;
}

void NaClFastMutexUnlock(struct NaClFastMutex *flp) {
  int ret = NaClMutexUnlock(&flp->mu); 
  UNREFERENCED_PARAMETER(ret);
}
