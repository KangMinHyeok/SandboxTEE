/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// https://locklessinc.com/articles/mutex_cv_futex/

#include "native_client/src/include/nacl_macros.h"
#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_sync.h"

#include <linux/futex.h>
#include "native_client/src/include/gcc/gatomic.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/trusted/stdlib/errno.h"


int NaClMutexCtor(struct NaClMutex *mp) {
  iatomic_set(&mp->mu, 0);
  return 1; // SUCCESS
}

void NaClMutexDtor(struct NaClMutex *mp) {
  
  int ret;
  ret = NaClMutexUnlock(mp);
  if (ret != NACL_SYNC_OK)
    NaClLog(LOG_FATAL, "NaClMutexDtor: dtor error\n");
    
}

#define MAP(E, S) case E: do { return S; } while (0)

#define MUTEX_SPINLOCK_TIMES    100
#define MUTEX_UNLOCKED            0
#define MUTEX_LOCKED              1
#define MUTEX_CONTENDED           2


NaClSyncStatus NaClMutexLock(struct NaClMutex *mp) {

  UNREFERENCED_PARAMETER(mp);
  int ret = 0;

  int i, c;

  for (i = 0; i< MUTEX_SPINLOCK_TIMES; i++){
    c = icmpxchg(&mp->mu, MUTEX_UNLOCKED, MUTEX_LOCKED);

    if (c == MUTEX_UNLOCKED){
      ret = NACL_SYNC_OK;
      goto out;
    }
    
    cpu_relax();  
  }

  if (c == MUTEX_LOCKED) c = ixchg(&mp->mu, MUTEX_CONTENDED);

  //debug_print("%s %d\n", __FUNCTION__, __LINE__);
  while(c)
  {
    // wait  in the kernel
    ocall_futex(&mp->mu, FUTEX_WAIT_PRIVATE, 2, NULL, NULL, 0);
    c = ixchg (&mp->mu, MUTEX_CONTENDED);
  }
  //debug_print("%s %d\n", __FUNCTION__, __LINE__);

  ret = NACL_SYNC_OK;

out: 
  switch (ret) {
    MAP(0, NACL_SYNC_OK);
    MAP(EINVAL, NACL_SYNC_MUTEX_INVALID);
    // no EAGAIN; we don't support recursive mutexes
    MAP(EDEADLK, NACL_SYNC_MUTEX_DEADLOCK);
  }
  
  return NACL_SYNC_INTERNAL_ERROR;
}


NaClSyncStatus NaClMutexUnlock(struct NaClMutex *mp) {
  UNREFERENCED_PARAMETER(mp);
  int ret;
  int i;

  if (mp->mu == MUTEX_CONTENDED)
    mp->mu = MUTEX_UNLOCKED;
  else if (ixchg (&mp->mu, MUTEX_UNLOCKED) == MUTEX_LOCKED)
 	return 0; 

  
  // i don understand why '*2'
  for (i = 0; i < MUTEX_SPINLOCK_TIMES * 2; i++){
    if (mp->mu){
      if (icmpxchg(&mp->mu, MUTEX_LOCKED, MUTEX_CONTENDED))
          return 0;

      cpu_relax();
    }
  }

  ocall_futex(&mp->mu, FUTEX_WAKE_PRIVATE, MUTEX_LOCKED, NULL, NULL, 0);
  ret = NACL_SYNC_OK;

  switch (ret) {
    MAP(0, NACL_SYNC_OK);
    MAP(EINVAL, NACL_SYNC_MUTEX_INVALID);
    MAP(EPERM, NACL_SYNC_MUTEX_PERMISSION);
  }
  return NACL_SYNC_INTERNAL_ERROR;
}
