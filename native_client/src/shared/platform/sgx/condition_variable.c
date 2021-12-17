/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "native_client/src/include/nacl_base.h"
#include "native_client/src/include/nacl_macros.h"
#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_sync.h"
#include "native_client/src/trusted/service_runtime/include/sys/time.h"

#include <linux/futex.h>
#include "native_client/src/include/gcc/gatomic.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"

//static const uint64_t kMicrosecondsPerSecond = 1000 * 1000;
//static const uint64_t kNanosecondsPerMicrosecond = 1000;

#  define INT_MAX 2147483647

/* Condition variable C API */

int NaClCondVarCtor(struct NaClCondVar  *cvp) {
  cvp->mu.mu = 0;
  cvp->seq = 0;
  /*if (0 != sgx_thread_cond_init(&cvp->cv, (sgx_thread_condattr_t *) 0)) {
    return 0;
  }*/
  return 1; // SUCCESS
}

void NaClCondVarDtor(struct NaClCondVar *cvp) {
  UNREFERENCED_PARAMETER(cvp);
  //sgx_thread_cond_destroy(&cvp->cv);
}

NaClSyncStatus NaClCondVarSignal(struct NaClCondVar *cvp) {
  iatomic_add(1, &cvp->seq);

  ocall_futex(&cvp->seq, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
  //sgx_thread_cond_signal(&cvp->cv);
  return NACL_SYNC_OK;
}

NaClSyncStatus NaClCondVarBroadcast(struct NaClCondVar *cvp) {
  if (!cvp->mu.mu) return NACL_SYNC_OK;

  iatomic_add(1, &cvp->seq);

  ocall_futex(&cvp->seq, FUTEX_REQUEUE_PRIVATE, 1, (void *)INT_MAX, &cvp->mu.mu, 0);

  //sgx_thread_cond_broadcast(&cvp->cv);
  return NACL_SYNC_OK;
}

NaClSyncStatus NaClCondVarWait(struct NaClCondVar *cvp,
                               struct NaClMutex   *mp) {
  int seq = cvp->seq;

  if (&cvp->mu != mp){
    if (cvp->mu.mu) return NACL_SYNC_MUTEX_INVALID;

    iatomic_cmpxchg(&cvp->mu.mu, 0, mp->mu);
    if (cvp->mu.mu != mp->mu) return NACL_SYNC_INTERNAL_ERROR;
  }

  int ret = NaClMutexUnlock(mp);

  if (ret != NACL_SYNC_OK)
    return NACL_SYNC_INTERNAL_ERROR;

  ocall_futex(&cvp->seq, FUTEX_WAIT_PRIVATE, seq, NULL, NULL, 0);

  while (iatomic_xchg(&mp->mu, 2)){
    ocall_futex(&mp->mu, FUTEX_WAIT_PRIVATE, 2, NULL, NULL, 0);
  }
  //sgx_thread_cond_wait(&cvp->cv, &mp->mu);
  return NACL_SYNC_OK;
}

NaClSyncStatus NaClCondVarTimedWaitRelative(
    struct NaClCondVar             *cvp,
    struct NaClMutex               *mp,
    NACL_TIMESPEC_T const          *reltime) {
    
  
  /*
  uint64_t relative_wait_us =
      reltime->tv_sec * kMicrosecondsPerSecond +
      reltime->tv_nsec / kNanosecondsPerMicrosecond;
  uint64_t current_time_us;
  uint64_t wakeup_time_us;
  int result;
  
  struct timespec ts;
  struct timeval tv;
  struct timezone tz = { 0, 0 };  // * UTC * /
  
  if (gettimeofday(&tv, &tz) == 0) {
    current_time_us = tv.tv_sec * kMicrosecondsPerSecond + tv.tv_usec;
  } else {
    return NACL_SYNC_INTERNAL_ERROR;
  }
  
  wakeup_time_us = current_time_us + relative_wait_us;
  ts.tv_sec = wakeup_time_us / kMicrosecondsPerSecond;
  ts.tv_nsec = (wakeup_time_us - ts.tv_sec * kMicrosecondsPerSecond) *
               kNanosecondsPerMicrosecond;
  
  result = -1; // pthread_cond_timedwait(&cvp->cv, &mp->mu, &ts);
  NaClLog(LOG_FATAL, "PAVE does not support timewait\n");
  if (0 == result) {
    return NACL_SYNC_OK;
  }
  */
  UNREFERENCED_PARAMETER(cvp);
  UNREFERENCED_PARAMETER(mp );
  UNREFERENCED_PARAMETER(reltime);
  return NACL_SYNC_CONDVAR_TIMEDOUT;
}

NaClSyncStatus NaClCondVarTimedWaitAbsolute(
    struct NaClCondVar              *cvp,
    struct NaClMutex                *mp,
    NACL_TIMESPEC_T const           *abstime) {
  /*
  //struct timespec ts;
  int result;
  //ts.tv_sec = abstime->tv_sec;
  //ts.tv_nsec = abstime->tv_nsec;
  result = -1; //pthread_cond_timedwait(&cvp->cv, &mp->mu, &ts);
  NaClLog(LOG_FATAL, "PAVE does not support timewait\n");

  if (0 == result) {
    return NACL_SYNC_OK;
  }
  */
  UNREFERENCED_PARAMETER(cvp);
  UNREFERENCED_PARAMETER(mp);
  UNREFERENCED_PARAMETER(abstime);
  return NACL_SYNC_CONDVAR_TIMEDOUT;
}
