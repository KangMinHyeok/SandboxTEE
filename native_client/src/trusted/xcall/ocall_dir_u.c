#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include "native_client/src/trusted/xcall/ocall_types.h"


int sgx_ocall_mkdir(void * pms) {
	ms_ocall_mkdir_t * ms = (ms_ocall_mkdir_t *) pms;
	int ret = 0;
	ret = mkdir(ms->ms_pathname, ms->ms_mode);
	return ret;
}
int sgx_ocall_rmdir(void * pms) {
	ms_ocall_rmdir_t * ms = (ms_ocall_rmdir_t *) pms;
	int ret = 0;
	ret = rmdir(ms->ms_pathname);
	return ret;
}

int sgx_ocall_chdir(void * pms) {
	ms_ocall_chdir_t * ms = (ms_ocall_chdir_t *) pms;
	int ret = 0;
	ret = chdir(ms->ms_pathname);
	return ret;
}

int sgx_ocall_getcwd(void * pms) {
	ms_ocall_getcwd_t * ms = (ms_ocall_getcwd_t *) pms;
	char * ret;
	ret = getcwd(ms->ms_path, ms->ms_size);
	if (ret == NULL)
		return -1;
	else
		return 0;
}

int sgx_ocall_link(void * pms) {
	ms_ocall_link_t * ms = (ms_ocall_link_t *) pms;
	int ret = 0;
	ret = link(ms->ms_oldpath, ms->ms_newpath);
	return ret;
}

int sgx_ocall_unlink(void * pms) {
	ms_ocall_unlink_t * ms = (ms_ocall_unlink_t *) pms;
	int ret = 0;
	ret = unlink(ms->ms_pathname);
	return ret;
}

int sgx_ocall_truncate(void * pms) {
	ms_ocall_truncate_t * ms = (ms_ocall_truncate_t *) pms;
	int ret = 0;
	ret = truncate(ms->ms_pathname, ms->ms_size);
	return ret;
}

int sgx_ocall_rename(void * pms) {
	ms_ocall_rename_t * ms = (ms_ocall_rename_t *) pms;
	int ret = 0;
	ret = rename(ms->ms_oldpath, ms->ms_newpath);
	return ret;
}

int sgx_ocall_chmod(void * pms) {
	ms_ocall_chmod_t * ms = (ms_ocall_chmod_t *) pms;
	int ret = 0;
	ret = chmod(ms->ms_path, ms->ms_mode);
	return ret;
}

int sgx_ocall_access(void * pms) {
	ms_ocall_access_t * ms = (ms_ocall_access_t *) pms;
	int ret = 0;
	ret = access(ms->ms_pathname, ms->ms_mode);
	return ret;
}

int sgx_ocall_fcntl(void * pms) {
	ms_ocall_fcntl_t * ms = (ms_ocall_fcntl_t *) pms;
	int ret = 0;
	ret = fcntl(ms->ms_fd, ms->ms_cmd, ms->ms_args);
	return ret;
}

int sgx_ocall_getdents(void * pms) {
	int ret = 0;
	ms_ocall_getdents_t * ms = (ms_ocall_getdents_t *) pms;
	ret = getdents64(ms->ms_fd, ms->ms_dirp, ms->ms_size);
	return ret;
}

