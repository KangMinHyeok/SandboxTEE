#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

#include "native_client/src/trusted/xcall/ocall_types.h"


int sgx_ocall_gettimeofday(void * pms) {
	ms_ocall_gettimeofday_t * ms = (ms_ocall_gettimeofday_t *) pms;
	struct timeval tv;
	int ret = 0;

	ret = gettimeofday(&tv, NULL);
	ms->ms_tv_sec = tv.tv_sec;
	ms->ms_tv_usec = tv.tv_usec;
	return ret;
}

int sgx_ocall_clock_getres(void * pms) {
	ms_ocall_clock_getres_t * ms = (ms_ocall_clock_getres_t *) pms;
	struct timespec ts;
	int ret = 0;

	ret = clock_getres(ms->ms_clk_id, &ts);
	ms->ms_tp_sec = ts.tv_sec;
	ms->ms_tp_nsec = ts.tv_nsec;
	return ret;
}

int sgx_ocall_clock_gettime(void * pms) {

	ms_ocall_clock_gettime_t * ms = (ms_ocall_clock_gettime_t *) pms;
	struct timespec ts;
	int ret = 0;

	ret = clock_gettime(ms->ms_clk_id, &ts);
	ms->ms_tp_sec = ts.tv_sec;
	ms->ms_tp_nsec = ts.tv_nsec;
	return ret;
}

int sgx_ocall_sleep(void * pms) {
	ms_ocall_sleep_t * ms = (ms_ocall_sleep_t *) pms;
	unsigned int sec;
	int ret = 0;

	if (!ms->ms_microsec) {
		ret = sched_yield();
		return ret;
	}

	sec = ms->ms_microsec / 1000000;
	ret = sleep(sec);
	if (ret != 0)
		ms->ms_microsec = ret * 1000000;
	return ret;
}

int sgx_ocall_nsleep(void * pms) {
	ms_ocall_nsleep_t * ms = (ms_ocall_nsleep_t *) pms;
	struct timespec req, rem;
	int ret = 0;

	if (!ms->ms_rqtp_sec || !ms->ms_rqtp_nsec) {
		ret = sched_yield();
		return ret;
	}
	
	req.tv_sec = ms->ms_rmtp_sec;
	req.tv_nsec = ms->ms_rmtp_nsec;
	ret = nanosleep(&req, &rem);
	if ((ret < 0) && (-1 * ret) == EINTR) {
		ms->ms_rqtp_sec = rem.tv_sec;
		ms->ms_rqtp_nsec = rem.tv_nsec;
	}
	return ret;
}


