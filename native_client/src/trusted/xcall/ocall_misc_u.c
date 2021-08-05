#include <alloca.h>
#include <linux/futex.h> 
#include <sys/syscall.h>  
#include <unistd.h>
#include <stddef.h>
#include <time.h>

#include "native_client/src/trusted/xcall/ocall_types.h"

int sgx_ocall_futex(void * pms) {
	ms_ocall_futex_t * ms = (ms_ocall_futex_t *) pms;
	int ret = 0;
	struct timespec * ts = NULL;
	if (ms->ms_timeout != OCALL_NO_TIMEOUT) {
		ts = alloca((size_t)sizeof(struct timespec));
		ts->tv_sec = ms->ms_timeout / 1000000;
		ts->tv_nsec = (ms->ms_timeout - ts->tv_sec * 1000000) * 1000;
	}
	ret = syscall(SYS_futex, ms->ms_futex, ms->ms_op, ms->ms_val, ts, NULL, 0);
	return ret;
}



