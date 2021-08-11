/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl Simple/secure ELF loader (NaCl SEL).
 */

#include "native_client/src/include/build_config.h"
#include "native_client/src/include/portability.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "native_client/src/include/elf_constants.h"
#include "native_client/src/include/elf.h"
#include "native_client/src/include/nacl_macros.h"
#include "native_client/src/include/win/mman.h"
#include "native_client/src/shared/platform/nacl_check.h"
#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_sync_checked.h"
#include "native_client/src/shared/platform/nacl_time.h"

#include "native_client/src/trusted/desc/nacl_desc_base.h"
#include "native_client/src/trusted/perf_counter/nacl_perf_counter.h"

#include "native_client/src/trusted/service_runtime/include/sys/errno.h"
#include "native_client/src/trusted/service_runtime/include/sys/fcntl.h"

#include "native_client/src/trusted/service_runtime/arch/sel_ldr_arch.h"
#include "native_client/src/trusted/service_runtime/elf_util.h"
#include "native_client/src/trusted/service_runtime/nacl_app_thread.h"
#include "native_client/src/trusted/service_runtime/nacl_signal.h"
#include "native_client/src/trusted/service_runtime/nacl_switch_to_app.h"
#include "native_client/src/trusted/service_runtime/nacl_syscall_common.h"
#include "native_client/src/trusted/service_runtime/nacl_text.h"
#include "native_client/src/trusted/service_runtime/sel_memory.h"
#include "native_client/src/trusted/service_runtime/sel_ldr.h"
#include "native_client/src/trusted/service_runtime/sel_util.h"
#include "native_client/src/trusted/service_runtime/sel_addrspace.h"

#include "native_client/src/trusted/stdlib/string.h"

int NaClReportExitStatus(struct NaClApp *nap, int exit_status) {
  NaClXMutexLock(&nap->mu);
  /*
   * If several threads are exiting/reporting signals at once, we should
   * let only one thread to pass through. This way we can use exit code
   * without synchronization once we know that running==0.
   */
  if (!nap->running) {
    NaClXMutexUnlock(&nap->mu);
    return 0;
  }

  nap->exit_status = exit_status;
  nap->running = 0;
  NaClXCondVarSignal(&nap->cv);

  NaClXMutexUnlock(&nap->mu);

  return 0;
}

uintptr_t NaClGetInitialStackTop(struct NaClApp *nap) {
  /*
   * We keep the top of useful memory a page below the top of the
   * sandbox region so that compilers can do tricks like computing a
   * base register of sp + constant and then using a
   * register-minus-constant addressing mode, which comes up at least
   * on ARM where the compiler is trying to optimize given the limited
   * size of immediate offsets available.  The maximum such negative
   * constant on ARM will be -4095, but we use page size (64k) for
   * good measure and do it on all machines just for uniformity.
   */
  return ((uintptr_t) 1U << nap->addr_bits) - NACL_MAP_PAGESIZE;
}

/*
 * preconditions:
 *  * argc is the length of the argv array
 *  * envv may be NULL (this happens on MacOS/Cocoa and in tests)
 *  * if envv is non-NULL it is 'consistent', null terminated etc.
 */
int NaClCreateMainThread(struct NaClApp     *nap,
                         int                argc,
                         char               **argv,
                         char const *const  *envv) {
  /*
   * Compute size of string tables for argv and envv
   */
  int                   retval;
  int                   envc;
  size_t                size;
  int                   auxv_entries;
  size_t                ptr_tbl_size;
  int                   i;
  uint32_t              *p;
  char                  *strp;
  size_t                *argv_len;
  size_t                *envv_len;
  uintptr_t             stack_ptr;

  retval = 0;  /* fail */
  CHECK(argc >= 0);
  CHECK(NULL != argv || 0 == argc);

  envc = 0;
  if (NULL != envv) {
    char const *const *pp;
    for (pp = envv; NULL != *pp; ++pp) {
      ++envc;
    }
  }
  envv_len = 0;
  argv_len = malloc(argc * sizeof argv_len[0]);
  envv_len = malloc(envc * sizeof envv_len[0]);
  if (NULL == argv_len) {
    goto cleanup;
  }
  if (NULL == envv_len && 0 != envc) {
    goto cleanup;
  }

  size = 0;

  /*
   * The following two loops cannot overflow.  The reason for this is
   * that they are counting the number of bytes used to hold the
   * NUL-terminated strings that comprise the argv and envv tables.
   * If the entire address space consisted of just those strings, then
   * the size variable would overflow; however, since there's the code
   * space required to hold the code below (and we are not targetting
   * Harvard architecture machines), at least one page holds code, not
   * data.  We are assuming that the caller is non-adversarial and the
   * code does not look like string data....
   */
  for (i = 0; i < argc; ++i) {
    argv_len[i] = strlen(argv[i]) + 1;
    size += argv_len[i];
  }
  for (i = 0; i < envc; ++i) {
    envv_len[i] = strlen(envv[i]) + 1;
    size += envv_len[i];
  }

  /*
   * NaCl modules are ILP32, so the argv, envv pointers, as well as
   * the terminating NULL pointers at the end of the argv/envv tables,
   * are 32-bit values.  We also have the auxv to take into account.
   *
   * The argv and envv pointer tables came from trusted code and is
   * part of memory.  Thus, by the same argument above, adding in
   * "ptr_tbl_size" cannot possibly overflow the "size" variable since
   * it is a size_t object.  However, the extra pointers for auxv and
   * the space for argv could cause an overflow.  The fact that we
   * used stack to get here etc means that ptr_tbl_size could not have
   * overflowed.
   *
   * NB: the underlying OS would have limited the amount of space used
   * for argv and envv -- on linux, it is ARG_MAX, or 128KB -- and
   * hence the overflow check is for obvious auditability rather than
   * for correctness.
   */
  auxv_entries = 1;
  if (0 != nap->user_entry_pt) {
    auxv_entries++;
  }
  if (0 != nap->dynamic_text_start) {
    auxv_entries++;
  }
  ptr_tbl_size = 3 + argc + 1 + envc + 1 + auxv_entries * 2;
#if NACL_STACK_GETS_ARG
  ptr_tbl_size++;
#endif
  ptr_tbl_size *= sizeof(uint32_t);

  if (SIZE_T_MAX - size < ptr_tbl_size) {
    NaClLog(LOG_WARNING,
            "NaClCreateMainThread: ptr_tbl_size cause size of"
            " argv / environment copy to overflow!?!\n");
    retval = 0;
    goto cleanup;
  }
  size += ptr_tbl_size;

  size = (size + NACL_STACK_ALIGN_MASK) & ~NACL_STACK_ALIGN_MASK;

  if (size > nap->stack_size) {
    retval = 0;
    goto cleanup;
  }

  /*
   * Write strings and char * arrays to stack.
   */
  stack_ptr = NaClUserToSysAddrRange(nap, NaClGetInitialStackTop(nap) - size,
                                     size);
  if (stack_ptr == kNaClBadAddress) {
    retval = 0;
    goto cleanup;
  }

  NaClLog(2, "setting stack to : %016"NACL_PRIxPTR"\n", stack_ptr);

  VCHECK(0 == (stack_ptr & NACL_STACK_ALIGN_MASK),
         ("stack_ptr not aligned: %016"NACL_PRIxPTR"\n", stack_ptr));

  p = (uint32_t *) stack_ptr;
  strp = (char *) stack_ptr + ptr_tbl_size;

  /*
   * For x86-32, we push an initial argument that is the address of
   * the main argument block.  For other machines, this is passed
   * in a register and that's set in NaClStartThreadInApp.
   */
#if NACL_STACK_GETS_ARG
  {
    uint32_t *argloc = p++;  /* Prevent unsequenced access to p in next line. */
    *argloc = (uint32_t) NaClSysToUser(nap, (uintptr_t) p);
  }
#endif

  *p++ = 0;  /* Cleanup function pointer, always NULL.  */
  *p++ = envc;
  *p++ = argc;

  for (i = 0; i < argc; ++i) {
    *p++ = (uint32_t) NaClSysToUser(nap, (uintptr_t) strp);
    NaClLog(2, "copying arg %d  %p -> %p\n",
            i, (void *) argv[i], (void *) strp);
    strcpy(strp, argv[i]);
    strp += argv_len[i];
  }
  *p++ = 0;  /* argv[argc] is NULL.  */

  for (i = 0; i < envc; ++i) {
    *p++ = (uint32_t) NaClSysToUser(nap, (uintptr_t) strp);
    NaClLog(2, "copying env %d  %p -> %p\n",
            i, (void *) envv[i], (void *) strp);
    strcpy(strp, envv[i]);
    strp += envv_len[i];
  }
  *p++ = 0;  /* envp[envc] is NULL.  */

  /* Push an auxv */
  if (0 != nap->user_entry_pt) {
    *p++ = AT_ENTRY;
    *p++ = (uint32_t) nap->user_entry_pt;
  }
  if (0 != nap->dynamic_text_start) {
    *p++ = AT_BASE;
    *p++ = (uint32_t) nap->dynamic_text_start;
  }
  *p++ = AT_NULL;
  *p++ = 0;

  CHECK((char *) p == (char *) stack_ptr + ptr_tbl_size);

  /* now actually spawn the thread */
  NaClXMutexLock(&nap->mu);
  /*
   * Unreference the main nexe and irt at this point if no debug stub callbacks
   * have been registered, as these references to the main nexe and irt
   * descriptors are only used when providing file access to the debugger.
   * In the debug case, let shutdown take care of cleanup.
   */
  if (NULL == nap->debug_stub_callbacks) {
    if (NULL != nap->main_nexe_desc) {
      NaClDescUnref(nap->main_nexe_desc);
      nap->main_nexe_desc = NULL;
    }
    if (NULL != nap->irt_nexe_desc) {
      NaClDescUnref(nap->irt_nexe_desc);
      nap->irt_nexe_desc = NULL;
    }
  }
  nap->running = 1;
  NaClXMutexUnlock(&nap->mu);

  /*
   * For x86, we adjust the stack pointer down to push a dummy return
   * address.  This happens after the stack pointer alignment.
   * We avoid the otherwise harmless call for the zero case because
   * _FORTIFY_SOURCE memset can warn about zero-length calls.
   */
#if NACL_STACK_PAD_BELOW_ALIGN != 0
  stack_ptr -= NACL_STACK_PAD_BELOW_ALIGN;
  memset((void *) stack_ptr, 0, NACL_STACK_PAD_BELOW_ALIGN);
#endif

  NaClLog(2, "system stack ptr : %016"NACL_PRIxPTR"\n", stack_ptr);
  NaClLog(2, "  user stack ptr : %016"NACL_PRIxPTR"\n",
          NaClSysToUserStackAddr(nap, stack_ptr));

  /* e_entry is user addr */
  retval = NaClAppThreadSpawn(nap,
                              nap->initial_entry_pt,
                              NaClSysToUserStackAddr(nap, stack_ptr),
                              /* user_tls1= */ (uint32_t) nap->break_addr,
                              /* user_tls2= */ 0);

cleanup:
  free(argv_len);
  free(envv_len);

  return retval;
}

int NaClWaitForMainThreadToExit(struct NaClApp  *nap) {
  NaClLog(3, "NaClWaitForMainThreadToExit: taking NaClApp lock\n");
  NaClXMutexLock(&nap->mu);
  NaClLog(3, " waiting for exit status\n");
  while (nap->running) {
    NaClXCondVarWait(&nap->cv, &nap->mu);
    NaClLog(3, " wakeup, nap->running %d, nap->exit_status %d\n",
            nap->running, nap->exit_status);
  }
  NaClXMutexUnlock(&nap->mu);
  /*
   * Some thread invoked the exit (exit_group) syscall.
   */

  if (NULL != nap->debug_stub_callbacks) {
    nap->debug_stub_callbacks->process_exit_hook();
  }

  return NACL_ABI_WEXITSTATUS(nap->exit_status);
}

/*
 * stack_ptr is from syscall, so a 32-bit address.
 */
int32_t NaClCreateAdditionalThread(struct NaClApp *nap,
                                   uintptr_t      prog_ctr,
                                   uintptr_t      sys_stack_ptr,
                                   uint32_t       user_tls1,
                                   uint32_t       user_tls2) {
  if (!NaClAppThreadSpawn(nap,
                          prog_ctr,
                          NaClSysToUserStackAddr(nap, sys_stack_ptr),
                          user_tls1,
                          user_tls2)) {
    NaClLog(LOG_WARNING,
            ("NaClCreateAdditionalThread: could not allocate thread."
             "  Returning EAGAIN per POSIX specs.\n"));
    return -NACL_ABI_EAGAIN;
  }
  return 0;
}
