#include <string.h>

#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"



int ocall_mkdir (const char * pathname, unsigned short mode) {
	int retval = 0;
	int len = pathname ? strlen(pathname) + 1 : 0;
	ms_ocall_mkdir_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_pathname = sgx_copy_to_ustack(pathname, len);
	ms->ms_mode = mode;

	retval = sgx_ocall(OCALL_MKDIR, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_rmdir (const char * pathname) {
	int retval = 0;
	int len = pathname ? strlen(pathname) + 1 : 0;
	ms_ocall_rmdir_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_pathname = sgx_copy_to_ustack(pathname, len);

	retval = sgx_ocall(OCALL_RMDIR, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_chdir (const char * pathname) {
	int retval = 0;
	int len = pathname ? strlen(pathname) + 1 : 0;
	ms_ocall_chdir_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_pathname = sgx_copy_to_ustack(pathname, len);

	retval = sgx_ocall(OCALL_CHDIR, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_getcwd (char * pathname, size_t size) {
	int retval = 0;
	ms_ocall_getcwd_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_size = size;

	retval = sgx_ocall(OCALL_GETCWD, ms);

	if (retval >= 0){
		if (ms->ms_path && ms->ms_pathlen) {
			sgx_copy_to_enclave(pathname, size, ms->ms_path, ms->ms_pathlen+1);
		}
	}

	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_link (const char * oldpath, const char * newpath) {
	int retval = 0;
	int oldlen = oldpath ? strlen(oldpath) + 1 : 0;
	int newlen = newpath ? strlen(newpath) + 1 : 0;
	ms_ocall_link_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_oldpath = sgx_copy_to_ustack(oldpath, oldlen);
	ms->ms_newpath = sgx_copy_to_ustack(newpath, newlen);

	retval = sgx_ocall(OCALL_LINK, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_unlink (const char * pathname) {
	int retval = 0;
	int len = pathname ? strlen(pathname) + 1 : 0;
	ms_ocall_unlink_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_pathname = sgx_copy_to_ustack(pathname, len);

	retval = sgx_ocall(OCALL_UNLINK, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}


int ocall_truncate (const char * pathname, unsigned int size) {
	int retval = 0;
	int len = pathname ? strlen(pathname) + 1 : 0;
	ms_ocall_truncate_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_pathname = sgx_copy_to_ustack(pathname, len);
	ms->ms_size = size;

	retval = sgx_ocall(OCALL_TRUNCATE, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_rename (const char * oldpath, const char * newpath) {
	int retval = 0;
	int oldlen = oldpath ? strlen(oldpath) + 1 : 0;
	int newlen = newpath ? strlen(newpath) + 1 : 0;

	ms_ocall_rename_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_oldpath = sgx_copy_to_ustack(oldpath, oldlen);
	ms->ms_newpath = sgx_copy_to_ustack(newpath, newlen);

	retval = sgx_ocall(OCALL_RENAME, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_chmod(const char * path, uint64_t mode) {
	int retval = 0;
	int len = path ? strlen(path) + 1 : 0;
	ms_ocall_chmod_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_path = sgx_copy_to_ustack(path, len);
	ms->ms_mode = mode;

	retval = sgx_ocall(OCALL_CHMOD, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_access (const char * pathname, unsigned short mode) { 
	int retval = 0;
	int len = pathname ? strlen(pathname) + 1 : 0;
	ms_ocall_access_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_pathname = sgx_copy_to_ustack(pathname, len);
	ms->ms_mode = mode;

	retval = sgx_ocall(OCALL_ACCESS, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fcntl(int fd, int cmd, long args) {
	int retval = 0;
	ms_ocall_fcntl_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_cmd = cmd;
	ms->ms_args = args;

	retval = sgx_ocall(OCALL_FCNTL, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_getdents (unsigned int fd, struct linux_dirent64 * dirp, unsigned int size) {
	int retval = 0;
	ms_ocall_getdents_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_dirp = sgx_alloc_on_ustack(size);
	ms->ms_size = size;

	retval = sgx_ocall(OCALL_GETDENTS, ms);
	if (retval > 0)
		sgx_copy_to_enclave(dirp, size, ms->ms_dirp, retval);
	sgx_reset_ustack(old_ustack);
	return retval;
}

