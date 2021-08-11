#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <alloca.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <time.h>

#include "native_client/src/trusted/xcall/ocall_types.h"


int sgx_ocall_exit(void * pms) {
	int64_t ms = (int64_t) pms;

	if (ms != (int64_t)((uint8_t) ms)) {
		printf("Saturation error in exit code %ld, getting rounded down to %u\n", ms, (uint8_t) ms);
		ms = 255;
	}

	syscall(SYS_exit_group, (int) ms);
	return 0;
}

int sgx_ocall_cpuid(void * pms) {
	ms_ocall_cpuid_t * ms = (ms_ocall_cpuid_t *) pms;
	__asm__ volatile ("cpuid"
			: "=a"(ms->ms_values[0]),
			"=b"(ms->ms_values[1]),
			"=c"(ms->ms_values[2]),
			"=d"(ms->ms_values[3])
			: "a"(ms->ms_leaf), "c"(ms->ms_subleaf) : "memory");
	return 0;
}

int sgx_ocall_getpid(void * pms) {
	ms_ocall_pid_t * ms = (ms_ocall_pid_t *) pms;
	ms = NULL;
	return getpid();
}

int sgx_ocall_fionread(void * pms) {
	ms_ocall_fionread_t * ms = (ms_ocall_fionread_t *) pms;
	int ret = 0;
	int val;
	ret = ioctl(ms->ms_fd, FIONREAD, &val);

	if (ret < 0)
		return ret;
	else
		return val;
}

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

int sgx_ocall_print_string(void * pms) {
	int ret = 0;
	ms_ocall_print_string_t * ms = (ms_ocall_print_string_t *) pms;
	ret = write(2, ms->ms_str, ms->ms_length);
	return ret;
}

int sgx_ocall_debugp(void * pms) {

	ms_ocall_debugp_t * ms = (ms_ocall_debugp_t *) pms;
	int val = ms->ms_val;
	printf("[DEBUG] %d\n", val);

	return 0;
} 
