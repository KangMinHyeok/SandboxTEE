/*
 * Copyright 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "native_client/src/shared/platform/nacl_exit.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"


#if NACL_SGX == 1
#include "native_client/src/trusted/stdlib/assert.h"
#else
#include <stdlib.h>
#include <stdio.h>
#endif
// #include <signal.h>
#include <unistd.h>


#ifndef SIGABRT
#define SIGABRT 6
#endif

void NaClAbort(void) {
  __abort();
}

void NaClExit(int err_code) {
  ocall_exit(err_code);
}

