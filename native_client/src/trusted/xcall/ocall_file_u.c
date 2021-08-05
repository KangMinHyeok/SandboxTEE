#include <fcntl.h>
#include <unistd.h>


#include "native_client/src/trusted/xcall/ocall_types.h"

int sgx_ocall_open(void * pms) {
	ms_ocall_open_t * ms = (ms_ocall_open_t *) pms;
	int ret = 0;
	ret = open(ms->ms_pathname, O_RDWR|O_CREAT|O_SYNC, 0666);
	return ret;
}

int sgx_ocall_close(void * pms) {
	int ret = 0;
	ms_ocall_close_t * ms = (ms_ocall_close_t *) pms;
	ret = close(ms->ms_fd);
	return ret;
}

int sgx_ocall_read(void * pms) {
	ms_ocall_read_t * ms = (ms_ocall_read_t *) pms;
	int ret = 0;
	ret = read(ms->ms_fd, ms->ms_buf, ms->ms_count);
	return ret;
}

int sgx_ocall_write(void * pms) {
	ms_ocall_write_t * ms = (ms_ocall_write_t *) pms;
	int ret = 0;
	ret = write(ms->ms_fd, ms->ms_buf, ms->ms_count);
	return ret; 
}


