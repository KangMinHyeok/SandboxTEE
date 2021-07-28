#include <stdio.h>
#include <errno.h>

#include "native_client/src/trusted/xcall/ocall_types.h"

int sgx_ocall_debugp(void * pms) {

	ms_ocall_debugp_t * ms = (ms_ocall_debugp_t *) pms;
	int val = ms->ms_val;
	printf("[DEBUG] %d\n", val);
	
	return 0;
}	




