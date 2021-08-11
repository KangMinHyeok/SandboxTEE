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
#include "native_client/src/include/portability_io.h"

// #include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "native_client/src/shared/platform/nacl_check.h"
#include "native_client/src/shared/platform/nacl_exit.h"
#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_sync.h"
#include "native_client/src/shared/platform/nacl_sync_checked.h"

#include "native_client/src/trusted/desc/nacl_desc_base.h"
#include "native_client/src/trusted/desc/nacl_desc_io.h"
#include "native_client/src/trusted/perf_counter/nacl_perf_counter.h"
#include "native_client/src/trusted/service_runtime/env_cleanser.h"
#include "native_client/src/trusted/fault_injection/fault_injection.h"
#include "native_client/src/trusted/fault_injection/test_injection.h"
#include "native_client/src/trusted/service_runtime/include/sys/fcntl.h"
#include "native_client/src/trusted/service_runtime/nacl_app.h"
#include "native_client/src/trusted/service_runtime/nacl_all_modules.h"
#include "native_client/src/trusted/service_runtime/nacl_debug_init.h"
#include "native_client/src/trusted/service_runtime/nacl_error_log_hook.h"
#include "native_client/src/trusted/service_runtime/nacl_globals.h"
// #include "native_client/src/trusted/service_runtime/nacl_signal.h"
#include "native_client/src/trusted/service_runtime/nacl_syscall_common.h"
#include "native_client/src/trusted/service_runtime/nacl_valgrind_hooks.h"
#include "native_client/src/trusted/service_runtime/sel_ldr.h"
#include "native_client/src/trusted/service_runtime/sel_main.h"
#include "native_client/src/trusted/service_runtime/sel_main_common.h"
#include "native_client/src/trusted/service_runtime/sel_qualify.h"


#include "native_client/src/trusted/stdlib/fileio.h"
//#include "native_client/src/trusted/stdlib/stdio.h"
/*
static void VmentryPrinter(void           *state,
                    struct NaClVmmapEntry *vmep) {
  UNREFERENCED_PARAMETER(state);
  printf("page num 0x%06x\n", (uint32_t)vmep->page_num);
  printf("num pages %d\n", (uint32_t)vmep->npages);
  printf("prot bits %x\n", vmep->prot);
  fflush(stdout);
}

static void PrintVmmap(struct NaClApp  *nap) {
  printf("In PrintVmmap\n");
  fflush(stdout);
  NaClXMutexLock(&nap->mu);
  NaClVmmapVisit(&nap->mem_map, VmentryPrinter, (void *) 0);

  NaClXMutexUnlock(&nap->mu);
}
*/


// untrusted
int NaClAppPrepareModuleInSGX(struct SelLdrOptions *options, struct NaClApp *nap) {

  //NaClErrorCode                 errcode = LOAD_INTERNAL;
  //struct NaClDesc               *blob_file = NULL;

  int                           ret_code;

  struct DynArray               env_vars;
  struct NaClEnvCleanser        env_cleanser;
  char const *const             *envp;


  ret_code = 1;

  NaClAllModulesInit();

  if (!DynArrayCtor(&env_vars, 0)) { // untrusted copy
    NaClLog(LOG_FATAL, "Failed to allocate env var array\n");
  }

  // TODO(copy options, env_vars, nap?)
  //NaClSelLdrParseArgs(argc, argv, options, &env_vars, nap); // untrusted copy (options, env_vars, nap)

  /*
   * Define the environment variables for untrusted code.
   */
  if (!DynArraySet(&env_vars, env_vars.num_entries, NULL)) {
    NaClLog(LOG_FATAL, "Adding env_vars NULL terminator failed\n");
  }

  NaClEnvCleanserCtor(&env_cleanser, 0, options->enable_env_passthrough); // untrusted copy
  if (!NaClEnvCleanserInit(&env_cleanser, NaClGetEnviron(),
                           (char const *const *) env_vars.ptr_array)) {
    NaClLog(LOG_FATAL, "Failed to initialise env cleanser\n");
  }
  envp = NaClEnvCleanserEnvironment(&env_cleanser); // untrusted


  if (options->debug_mode_bypass_acl_checks) {
    /* If both -m and -a are specified, -m takes precedence. */
    NaClInsecurelyBypassAllAclChecks(); // delete
  }

	// Already copied from USGx
  // nap->ignore_validator_result = (options->debug_mode_ignore_validator > 0);
  // nap->skip_validator = (options->debug_mode_ignore_validator > 1);
  // nap->enable_exception_handling = options->enable_exception_handling;

  // errcode = LOAD_OK;

  /*
   * in order to report load error to the browser plugin through the
   * secure command channel, we do not immediate jump to cleanup code
   * on error.  rather, we continue processing (assuming earlier
   * errors do not make it inappropriate) until the secure command
   * channel is set up, and then bail out.
   */

	// TODO (mkpark): how to handle signal?
  // NaClSignalHandlerInit(); // ildan pass
  // NaClSignalTestCrashOnStartup(); // ildan pass

  /*
   * Open both files first because (on Mac OS X at least)
   * NaClAppLoadFile() enables an outer sandbox.
   */
  /*
  if (NULL != options->blob_library_file) { // delete
    NaClFileNameForValgrind(options->blob_library_file);
    blob_file = (struct NaClDesc *) NaClDescIoDescOpen(
        options->blob_library_file, NACL_ABI_O_RDONLY, 0);
    if (NULL == blob_file) {
      perror("sel_main");
      NaClLog(LOG_FATAL, "Cannot open \"%s\".\n", options->blob_library_file);
    }
  }
	*/
  NaClAppInitialDescriptorHookup(nap); // untrusted + trusted

	// TODO(mkpark): delete the below commented code
	/* 
  NaClLog(2, "Loading nacl file %s (non-RPC)\n", options->nacl_file);

  errcode = NaClAppLoadFileFromFilename(nap, options->nacl_file); // need modification (LOAD enclave)
  if (LOAD_OK != errcode) {
    if (!options->quiet) {
      NaClLog(LOG_ERROR, "Error while loading \"%s\": %s\n"
              "Using the wrong type of nexe (nacl-x86-32"
              " on an x86-64 or vice versa)\n"
              "or a corrupt nexe file may be"
              " responsible for this error.\n",
              options->nacl_file,
              NaClErrorString(errcode));
    }
    goto error;
  }
  */


  /*
   * Enable the outer sandbox, if one is defined.  Do this as soon as
   * possible, but after we have opened files.
   *
   * We cannot enable the sandbox if file access is enabled.
   */

	/*
	// load already done
  if (NULL != options->blob_library_file) { // delete
    errcode = NaClMainLoadIrt(nap, blob_file, NULL);
    if (LOAD_OK != errcode) {
      NaClLog(LOG_ERROR, "Error while loading \"%s\": %s\n",
              options->blob_library_file,
              NaClErrorString(errcode));
      goto error;
    }

    NaClDescUnref(blob_file);
  }
  */


  /*
   * Make sure all the file buffers are flushed before entering
   * the application code.
   */
  fflush((FILE *) NULL);

  NaClAppStartModule(nap); // untrusted

  /*
   * For restricted file access, change directory to the root of the restricted
   * directory. This is required for safety, because we allow relative
   * pathnames.
   */
  /* // Already done in usgx
  if (NaClRootDir != NULL && NaClHostDescChdir(NaClRootDir)) { // ignore
    NaClLog(LOG_FATAL, "Could not change directory to root dir\n");
  }
  */


	/* unsupported
  if (options->enable_debug_stub) {
    if (!NaClDebugInit(nap)) { // delete
      goto error;
    }
  }
  */
  
  // TODO(mkpark): check the below
  //NACL_TEST_INJECTION(BeforeMainThreadLaunches, ()); // ildan pass

  if (!NaClCreateMainThread(nap,
                            options->app_argc,
                            options->app_argv,
                            envp)) {
    NaClLog(LOG_FATAL, "creating main thread failed\n");
  }

  // NEVER REACH HERE

  /*
   * Clean up temp storage for env vars.
   */
  NaClEnvCleanserDtor(&env_cleanser);
  DynArrayDtor(&env_vars);


  ret_code = NaClWaitForMainThreadToExit(nap);


  /*
   * exit_group or equiv kills any still running threads while module
   * addr space is still valid.  otherwise we'd have to kill threads
   * before we clean up the address space.
   */
  NaClExit(ret_code);

 //error:
  // mhkang
  fflush(stdout);

  if (options->verbosity > 0) {
    printf("Dumping vmmap.\n");
    
    fflush(stdout);
    // PrintVmmap(nap);
    fflush(stdout);
  }

  if (options->verbosity > 0) {
    printf("Done.\n");
  }
  fflush(stdout);

  // NaClSignalHandlerFini();
  NaClAllModulesFini();

  NaClExit(ret_code);

  /* Unreachable, but having the return prevents a compiler error. */
  return ret_code;
}
