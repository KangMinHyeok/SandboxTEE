
#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"

// TODO(mkpark): below struct should be changed
#include "/usr/include/errno.h"
#include "/usr/include/time.h"
#include "/usr/include/x86_64-linux-gnu/sys/time.h"


int ocall_gettimeofday (struct timeval *tv)
{
    int retval = 0;
    ms_ocall_gettimeofday_t * ms;
    void* old_ustack = sgx_prepare_ustack();
    ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

    retval = sgx_ocall(OCALL_GETTIMEOFDAY, ms);
    if (!retval){
        tv->tv_sec = ms->ms_tv_sec;
        tv->tv_usec = ms->ms_tv_usec;
    }
    sgx_reset_ustack(old_ustack);
    return retval;
}
