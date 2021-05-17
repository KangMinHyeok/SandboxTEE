#include <errno.h>
#include <sys/time.h>
#include <time.h>

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


