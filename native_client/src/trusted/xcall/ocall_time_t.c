
#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"

// TODO(mkpark): below struct should be changed
#include <errno.h>
#include <time.h>


int ocall_gettimeofday (struct timeval *tv) {
    int retval = 0;
    ms_ocall_gettimeofday_t * ms;
    void * old_ustack = sgx_prepare_ustack();
    ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

    retval = sgx_ocall(OCALL_GETTIMEOFDAY, ms);
    if (!retval) {
        tv->tv_sec = ms->ms_tv_sec;
        tv->tv_usec = ms->ms_tv_usec;
    }
    sgx_reset_ustack(old_ustack);
    return retval;
}

int ocall_clock_gettime (int clk_id, struct timespec *tp) {
	int retval = 0;
	ms_ocall_clock_gettime_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));
	ms->ms_clk_id = clk_id;

	retval = sgx_ocall(OCALL_CLOCK_GETTIME, ms);
	if (!retval) {
		tp->tv_sec = ms->ms_tp_sec;
		tp->tv_nsec = ms->ms_tp_nsec;
	}
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_sleep(unsigned long * microsec) {
	int retval = 0;
	ms_ocall_sleep_t * ms;
	void * old_ustack = sgx_prepare_ustack();

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));
	ms->ms_microsec = microsec ? *microsec : 0;
	retval = sgx_ocall(OCALL_SLEEP, ms);

	if (microsec) {
		if (!retval) 
			*microsec = 0;
		else if (retval == -EINTR)
			*microsec = ms->ms_microsec;
	}
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_nsleep(struct timespec *rqtp, struct timespec *rmtp) {
	int retval = 0;
	ms_ocall_nsleep_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_rqtp_sec = rqtp->tv_sec;
	ms->ms_rqtp_nsec = rqtp->tv_nsec;

	retval = sgx_ocall(OCALL_NSLEEP, ms);
	if (retval >= 0) {
		rmtp->tv_sec = ms->ms_rmtp_sec;
		rmtp->tv_nsec = ms->ms_rmtp_nsec;
	} else if (retval == -EINTR) {
		rmtp->tv_sec = ms->ms_rqtp_sec;
		rmtp->tv_nsec = ms->ms_rqtp_nsec;
	}
	sgx_reset_ustack(old_ustack);
	return retval;
}


