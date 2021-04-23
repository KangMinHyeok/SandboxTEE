/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl Server Runtime logging code.
 */
#include "native_client/src/include/nacl_compiler_annotations.h"
#include "native_client/src/include/portability.h"
#include "native_client/src/include/portability_io.h"
#include "native_client/src/include/portability_process.h"
#include "native_client/src/include/portability_string.h"

#include <stdarg.h>
#include <limits.h>

#if NACL_PAVE
#include "native_client/src/trusted/sgxlib/enclave_ocalls.h"
// #include "native_client/src/trusted/stdlib/enclave_files.h"
#else
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_log_intern.h"

#define NACL_LOG_DEFAULT 8

#define THREAD_SAFE_DETAIL_CHECK  0
/*
 * If set, check detail_level without grabbing a mutex.  This makes
 * logging much cheaper, but implies that the verbosity level should
 * only be changed prior to going multithreaded.
 */

#include "native_client/src/shared/gio/gio.h"
#include "native_client/src/shared/platform/nacl_exit.h"
#include "native_client/src/shared/platform/nacl_sync.h"
#include "native_client/src/shared/platform/nacl_sync_checked.h"
#include "native_client/src/shared/platform/nacl_threads.h"
#include "native_client/src/shared/platform/nacl_timestamp.h"

static int              g_initialized = 0;

/*
 * All logging output is protected by this mutex, so that the logging
 * code may output using multiple calls to the gio functions and not
 * have logging output from multiple threads get intermixed.
 */
static struct NaClMutex log_mu;
static int              tag_output = 0;
static int              g_abort_count = 0;
/*
 * g_abort_count is incremented prior to calling
 * gNaClLogAbortBehavior, so that the abort hook can invoke other
 * functions that might need to log, without those log entries
 * deadlocking or polluting the crash log output.
 */
static int              g_abort_behavior_active = 0;


static int              verbosity = LOG_WARNING; // NACL_LOG_DEFAULT; // NACL_VERBOSITY_UNSET;
// static struct Gio       *nacl_log_stream = NULL;
// static struct GioFile   log_file_stream;
static int              timestamp_enabled = 0;



void NaClLogModuleInit(void) {
  // FILE            *log_iob;
  // struct Gio *stream;
  
  if (g_initialized != 0 && g_initialized != 1)
    g_initialized = 0;

  if (!g_initialized) {
    NaClXMutexCtor(&log_mu);
    g_initialized = 1;
  }

  tag_output = 0;
  g_abort_count = 0;
  g_abort_behavior_active = 0;
  verbosity = NACL_LOG_DEFAULT; 
  timestamp_enabled = 0;


  // NaClLogLock();
  // log_iob = NaClLogDupFileIo(stderr);
  // stream = NaClLogGioFromFileIoBuffer(log_iob);
  // NaClLogSetGio_mu(stream);
  // NaClLogUnlock();
}

void NaClLogModuleFini(void) {
  NaClMutexDtor(&log_mu);
  g_initialized = 0;
}

void NaClLogTagNext_mu(void) {
  tag_output = 1;
}

void NaClLogLock(void) {
  NaClXMutexLock(&log_mu);
  NaClLogTagNext_mu();
}

void NaClLogUnlock(void) {
  int run_abort_behavior = 0;
  switch (g_abort_count) {
    case 0:
      NaClXMutexUnlock(&log_mu);
      break;
    case 1:
      /*
       * include an easy-to-recognize output for the fuzzer to recognize
       */
      if (!g_abort_behavior_active) {
        NaClLog_mu(LOG_ERROR, "LOG_FATAL abort exit\n");
        g_abort_behavior_active = 1;
        run_abort_behavior = 1;
        /*
         * run abort behavior only on edge transition when
         * g_abort_behavior_active is first set.
         */
      }
      NaClXMutexUnlock(&log_mu);
      if (run_abort_behavior) {
        NaClAbort();  /* help coverity figure out that this is the default */
        /* The abort behavior hook may not abort, so abort here in case. */
        NaClAbort();
      }
      break;
    default:
      /*
       * Abort handling code in turn aborted.  Eeep!
       */
      NaClAbort();
      break;
  }
}


int NaClLogGetVerbosity(void) {
  int v;

  NaClLogLock();
  v = verbosity;
  NaClLogUnlock();

  return v;
}


void NaClLogEnableTimestamp(void) {
  timestamp_enabled = 1;
}

void NaClLogDisableTimestamp(void) {
  timestamp_enabled = 0;
}

/*
 * Output a printf-style formatted message if the log verbosity level
 * is set higher than the log output's detail level.  Note that since
 * this is not a macro, log message arguments that have side effects
 * will have their side effects regardless of whether the
 * corresponding log message is printed or not.  This is good from a
 * consistency point of view, but it means that should a logging
 * argument be expensive to compute, the log statement needs to be
 * surrounded by something like
 *
 *  if (detail_level <= NaClLogGetVerbosity()) {
 *    NaClLog(detail_level, "format string", expensive_arg(), ...);
 *  }
 *
 * The log message, if written, is prepended by a microsecond
 * resolution timestamp on linux and a millisecond resolution
 * timestamp on windows.  This means that if the NaCl app can read its
 * own logs, it can distinguish which host OS it is running on.
 */
void NaClLogDoLogV_mu(int         detail_level,
                      char const  *fmt,
                      va_list     ap) {
  char timestamp[128];
  int  pid;

  if (0 == g_abort_count) {
    if (timestamp_enabled && tag_output) {
      pid = 1; // temp
      debug_print("[%d,%u:%s] ",
              pid,
              NaClThreadId(),
              NaClTimeStampString(timestamp, sizeof timestamp));
      tag_output = 0;
    }
    (void) debug_print(fmt, ap);
    // (void) (*s->vtbl->Flush)(s);
  } else {
    debug_print("POST-ABORT: ");
    debug_print(fmt, ap);
  }

  if (LOG_FATAL == detail_level) {
    ++g_abort_count;
  }
}

void NaClLogV_mu(int        detail_level,
                 char const *fmt,
                 va_list    ap) {

  if (detail_level <= verbosity) {
    NaClLogDoLogV_mu(detail_level, fmt, ap);
  }
}

void NaClLogV(int         detail_level,
              char const  *fmt,
              va_list     ap) {
#if !THREAD_SAFE_DETAIL_CHECK
  if (detail_level > verbosity) {
    return;
  }
#endif
  NaClLogLock();
  NaClLogV_mu(detail_level, fmt, ap);
  NaClLogUnlock();
}

void NaClLog(int         detail_level,
             char const  *fmt,
             ...) {
/*  va_list ap;

  if (detail_level > verbosity) {
    return;
  }

  NaClLogLock();
  va_start(ap, fmt);
  //NaClLogV_mu(detail_level, fmt, ap);
  vprintf(fmt, &ap);
  va_end(ap);
  NaClLogUnlock(); */
}

void NaClLog_mu(int         detail_level,
                char const  *fmt,
                ...) {
  va_list ap;

#if THREAD_SAFE_DETAIL_CHECK
  if (detail_level > verbosity) {
    return;
  }
#endif

  va_start(ap, fmt);
  NaClLogV_mu(detail_level, fmt, ap);
  va_end(ap);
}

