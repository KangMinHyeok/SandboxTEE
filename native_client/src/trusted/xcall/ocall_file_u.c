#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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

int sgx_ocall_stat(void * pms) {
	ms_ocall_stat_t * ms = (ms_ocall_stat_t *) pms;
	int ret = 0;
	ret = stat(ms->ms_path, ms->ms_stat);
	return ret;
}

int sgx_ocall_lstat(void * pms) {
	ms_ocall_lstat_t * ms = (ms_ocall_lstat_t *) pms;
	int ret = 0;
	ret = lstat(ms->ms_path, ms->ms_stat);
	return ret;
}

int sgx_ocall_fstat(void * pms) {
	ms_ocall_fstat_t * ms = (ms_ocall_fstat_t *) pms;
	int ret = 0;
	ret = fstat(ms->ms_fd, ms->ms_stat);
	return ret;
}

int sgx_ocall_fsetnonblock(void * pms) {
	ms_ocall_fsetnonblock_t * ms = (ms_ocall_fsetnonblock_t *) pms;
	int ret = 0;
	int flags;

	ret = fcntl(ms->ms_fd, F_GETFL);
	if (ret < 0)
		return ret;

	flags = ret;
	if (ms->ms_nonblocking) {
		if (!(flags & O_NONBLOCK))
			ret = fcntl(ms->ms_fd, F_SETFL,	flags | O_NONBLOCK);
	} else {
		if (flags & O_NONBLOCK)
			ret = fcntl(ms->ms_fd, F_SETFL, flags & ~O_NONBLOCK);
	}

	if (ret < 0)
		return ret;
	else
		return 0;
}

int sgx_ocall_fchmod(void * pms) {
	ms_ocall_fchmod_t * ms = (ms_ocall_fchmod_t *) pms;
	int ret = 0;
	ret = fchmod(ms->ms_fd, ms->ms_mode);
	return ret;
}

int sgx_ocall_fchdir(void * pms) {
	ms_ocall_fchdir_t * ms = (ms_ocall_fchdir_t *) pms;
	int ret = 0;
	ret = fchdir(ms->ms_fd);
	return ret;
}

int sgx_ocall_fsync(void * pms) {
	ms_ocall_fsync_t * ms = (ms_ocall_fsync_t *) pms;
	int ret = 0;
	ret = fsync(ms->ms_fd);
	return ret;
}

int sgx_ocall_fdatasync(void * pms) {
	ms_ocall_fdatasync_t * ms = (ms_ocall_fdatasync_t *) pms;
	int ret = 0;
	ret = fdatasync(ms->ms_fd);
	return ret;
}

int sgx_ocall_ftruncate(void * pms) {
	ms_ocall_ftruncate_t * ms = (ms_ocall_ftruncate_t *) pms;
	int ret = 0;
	ret = ftruncate(ms->ms_fd, ms->ms_length);
	return ret;
}

int sgx_ocall_lseek64(void * pms) {
	ms_ocall_lseek64_t * ms = (ms_ocall_lseek64_t *) pms;
	int ret = 0;
	ret = lseek(ms->ms_fd, ms->ms_offset, ms->ms_whence);
	return ret;
}

int sgx_ocall_pread64(void * pms) {
	ms_ocall_pread64_t * ms = (ms_ocall_pread64_t *) pms;
	int ret = 0;
	ret = pread64(ms->ms_fd, ms->ms_buf, ms->ms_count, ms->ms_pos);
	return ret;
}

int sgx_ocall_pwrite64(void * pms) {
	ms_ocall_pwrite64_t * ms = (ms_ocall_pwrite64_t *) pms;
	int ret = 0;
	ret = pwrite64(ms->ms_fd, ms->ms_buf, ms->ms_count, ms->ms_offset);
	return ret;
}

int sgx_ocall_dup(void * pms) {
	ms_ocall_dup_t * ms = (ms_ocall_dup_t *) pms;
	int ret = 0;
	ret = dup(ms->ms_fd);
	return ret;
}

