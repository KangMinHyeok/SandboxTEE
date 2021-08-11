/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl Server Runtime threads implementation layer.
 */

/*
 * We need sys/mman.h for PAGE_SIZE on Android.  PAGE_SIZE is what
 * PTHREAD_STACK_MIN defined to, so Android's pthread.h is somewhat
 * buggy in that regard.
 */
//#include <sys/mman.h>
//#include <sys/types.h>
//#include <signal.h>
#include <limits.h>
/*
 * PTHREAD_STACK_MIN should come from pthread.h as documented, but is
 * actually pulled in by limits.h.
 */

#include "native_client/src/include/portability.h"
#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_threads.h"
#include "native_client/src/trusted/service_runtime/nacl_config.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"


/*
#if !defined(__native_client__) && NACL_KERN_STACK_SIZE < PTHREAD_STACK_MIN
# error "NaCl service runtime stack size is smaller than PTHREAD_STACK_MIN"
#endif
*/
static int NaClThreadCreate(struct NaClThread  *ntp,
                            void               (*start_fn)(void *),
                            void               *state,
                            size_t             stack_size,
                            int                is_detached) {
  
  int             code;
  int             rv;

  rv = 0;

  UNREFERENCED_PARAMETER(ntp);
  UNREFERENCED_PARAMETER(start_fn);
  UNREFERENCED_PARAMETER(state);
  UNREFERENCED_PARAMETER(stack_size);
  UNREFERENCED_PARAMETER(is_detached);
  UNREFERENCED_PARAMETER(code);

  NaClLog(LOG_FATAL,
            "nacl_thread: not provide multithreading\n");
  /*
  if (0 != (code = sgx_thread_create(&ntp->tid,
                                  &attr,
                                  (void *(*)(void *)) start_fn,
                                  state))) {
    NaClLog(LOG_ERROR,
            "nacl_thread: pthread_create returned %d\n",
            code);
    goto done;
  }
  rv = 1;

 done:
  */
  return rv;
}

/*
 * Even if ctor fails, it should be okay -- and required -- to invoke
 * the dtor on it.
 */
int NaClThreadCtor(struct NaClThread  *ntp,
                   void               (*start_fn)(void *),
                   void               *state,
                   size_t             stack_size) {
  return NaClThreadCreate(ntp, start_fn, state, stack_size,
                          /* is_detached= */ 1);
}


void NaClThreadDtor(struct NaClThread *ntp) {
  /*
   * The threads that we create are not joinable, and we cannot tell
   * when they are truly gone.  Fortunately, the threads themselves
   * and the underlying thread library are responsible for ensuring
   * that resources such as the thread stack are properly released.
   */
  UNREFERENCED_PARAMETER(ntp);
}

void NaClThreadExit(void) {
  ocall_exit(0);
}

void NaClThreadYield(void) {
  ocall_sleep(NULL);
}
