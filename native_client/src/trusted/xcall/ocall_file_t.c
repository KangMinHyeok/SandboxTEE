#include <string.h>
#include <sys/stat.h>


#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_common.h"


// TODO must modify
unsigned long pagesize = PRESET_PAGESIZE;
unsigned long pageshift = PRESET_PAGESIZE - 1;
unsigned long pagemask = ~(PRESET_PAGESIZE - 1); 

#define MAX_UNTRUSTED_STACK_BUF 128*PRESET_PAGESIZE

int ocall_open (const char * pathname, int flags, unsigned short mode) {
	int retval = 0;
	int len = pathname ? strlen(pathname) + 1 : 0;
	ms_ocall_open_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_pathname = sgx_copy_to_ustack(pathname, len);
	ms->ms_flags = flags;
	ms->ms_mode = mode;

	retval = sgx_ocall(OCALL_OPEN, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_close (int fd) {
	int retval = 0;
	ms_ocall_close_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;

	retval = sgx_ocall(OCALL_CLOSE, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_read (int fd, void * buf, unsigned int count) {
	int retval = 0;
	void * obuf = NULL;
	ms_ocall_read_t *ms;
	void * old_ustack = sgx_prepare_ustack();

	if (count > MAX_UNTRUSTED_STACK_BUF) {
		retval = ocall_alloc_untrusted(ALLOC_ALIGNUP(count), &obuf);
		if (retval < 0)
			return retval;
	}

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));
	if (!ms) {
		retval = -1;
		goto out;
	}

	ms->ms_fd = fd;
	ms->ms_count = count;
	if (obuf)
		ms->ms_buf = obuf;
	else
		ms->ms_buf = sgx_alloc_on_ustack(count);

	retval = sgx_ocall(OCALL_READ, ms);

	if (retval > 0) {
		if ((size_t) retval > count) {
			retval = -1;
			goto out;
		}
		if (!sgx_copy_to_enclave(buf, count, ms->ms_buf, retval)) {
			retval = -1;
			goto out;
		}
	}

out:
	sgx_reset_ustack(old_ustack);
	if (obuf)
		ocall_unmap_untrusted(obuf, ALLOC_ALIGNUP(count));

	return retval;
}

int ocall_write (int fd, const void * buf, unsigned int count) {
	int retval = 0;
	void * obuf = NULL;
	ms_ocall_write_t *ms;
	void * old_ustack = sgx_prepare_ustack();

	if (count > MAX_UNTRUSTED_STACK_BUF) {
		retval = ocall_alloc_untrusted(ALLOC_ALIGNUP(count), &obuf); //  
		if (retval < 0)
			return retval;
	}

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	if (obuf) {
		ms->ms_buf = obuf;
		memcpy(obuf, buf, count);
	} else {
		ms->ms_buf = sgx_copy_to_ustack(buf, count);
	}
	ms->ms_count = count;

	retval = sgx_ocall(OCALL_WRITE, ms);

	if (retval > 0 && (size_t) retval > count) {
		retval = -1;
		goto out;
	}

out:
	sgx_reset_ustack(old_ustack);
	if (obuf)
		ocall_unmap_untrusted(obuf, ALLOC_ALIGNUP(count));
	return retval;
}

int ocall_stat (const char * path, struct stat * buf) {
	int retval = 0;
	ms_ocall_stat_t * ms;
	int len = path ? strlen(path) + 1 : 0;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_path = sgx_copy_to_ustack(path, len);
	ms->ms_stat = sgx_alloc_on_ustack_aligned(sizeof(stat), alignof(stat));

	retval = sgx_ocall(OCALL_STAT, ms);
	if (!retval)
		memcpy(buf, &ms->ms_stat, sizeof(struct stat));
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_lstat (const char * path, struct stat * buf) {
	int retval = 0;
	ms_ocall_lstat_t * ms;
	int len = path ? strlen(path) + 1 : 0;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_path = sgx_copy_to_ustack(path, len);
	ms->ms_stat = sgx_alloc_on_ustack_aligned(sizeof(stat), alignof(stat));

	retval = sgx_ocall(OCALL_LSTAT, ms);
	if (!retval)
		memcpy(buf, &ms->ms_stat, sizeof(struct stat));
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fstat (int fd, struct stat * buf) {
	int retval = 0;
	ms_ocall_fstat_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_stat = sgx_alloc_on_ustack_aligned(sizeof(stat), alignof(stat));

	retval = sgx_ocall(OCALL_FSTAT, ms);
	if (!retval)
		memcpy(buf, &ms->ms_stat, sizeof(struct stat));
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_stat64 (const char * path, struct stat64 * buf) {
	int retval = 0;
	ms_ocall_stat64_t * ms;
	int len = path ? strlen(path) + 1 : 0;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_path = sgx_copy_to_ustack(path, len);
	ms->ms_stat = sgx_alloc_on_ustack_aligned(sizeof(stat64), alignof(stat64));

	retval = sgx_ocall(OCALL_STAT64, ms);
	if (!retval)
		memcpy(buf, &ms->ms_stat, sizeof(struct stat64));
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_lstat64 (const char * path, struct stat64 * buf) {
	int retval = 0;
	ms_ocall_lstat64_t * ms;
	int len = path ? strlen(path) + 1 : 0;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_path = sgx_copy_to_ustack(path, len);
	ms->ms_stat = sgx_alloc_on_ustack_aligned(sizeof(stat64), alignof(stat64));

	retval = sgx_ocall(OCALL_LSTAT64, ms);
	if (!retval)
		memcpy(buf, &ms->ms_stat, sizeof(struct stat64));
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fstat64 (int fd, struct stat64 * buf) {
	int retval = 0;
	ms_ocall_fstat64_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_stat = sgx_alloc_on_ustack_aligned(sizeof(stat64), alignof(stat64));

	retval = sgx_ocall(OCALL_FSTAT64, ms);
	if (!retval)
		memcpy(buf, ms->ms_stat, sizeof(struct stat64));
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fsetnonblock (int fd, int nonblocking) {
	int retval = 0; 
	ms_ocall_fsetnonblock_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_nonblocking = nonblocking;

	retval = sgx_ocall(OCALL_FSETNONBLOCK, ms); 
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fchmod (int fd, unsigned short mode) {
	int retval = 0; 
	ms_ocall_fchmod_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_mode = mode;

	retval = sgx_ocall(OCALL_FCHMOD, ms); 
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fchdir (int fd) {
	int retval = 0; 
	ms_ocall_fchdir_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;

	retval = sgx_ocall(OCALL_FCHDIR, ms); 
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fsync (int fd) {
	int retval = 0;
	ms_ocall_fsync_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;

	retval = sgx_ocall(OCALL_FSYNC, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_fdatasync (int fd) {
	int retval = 0;
	ms_ocall_fdatasync_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;

	retval = sgx_ocall(OCALL_FDATASYNC, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_ftruncate (int fd, uint64_t length) {
	int retval = 0;
	ms_ocall_ftruncate_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_length = length;

	retval = sgx_ocall(OCALL_FTRUNCATE, ms);
	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_lseek64(int fd, long int offset, int whence) {
	int retval = 0;

	ms_ocall_lseek64_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	ms->ms_offset = offset;
	ms->ms_whence = whence;

	retval = sgx_ocall(OCALL_LSEEK64, ms);

	sgx_reset_ustack(old_ustack);
	return retval;
}

int ocall_pread64 (int fd, void * buf, size_t count, unsigned long pos) {
	int retval = 0;
	void * obuf = NULL;
	ms_ocall_pread64_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	
	if (count > 4096) {
		retval = ocall_alloc_untrusted(ALLOC_ALIGNUP(count), &obuf);
		if (retval < 0)
			return retval;
	}

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	if (obuf)
		ms->ms_buf = obuf;
	else
		ms->ms_buf = sgx_alloc_on_ustack(count);
	ms->ms_count = count;
	ms->ms_pos = pos;

	retval = sgx_ocall(OCALL_PREAD64, ms);

	if (retval > 0)
		memcpy(buf, ms->ms_buf, retval);
	sgx_reset_ustack(old_ustack);

	if (obuf)
		ocall_unmap_untrusted(obuf, ALLOC_ALIGNUP(count));

	return retval;

}

int ocall_pwrite64 (int fd, const void * buf, size_t count, unsigned long offset) {
	int retval = 0;
	void * obuf = NULL;
	void * old_ustack = sgx_prepare_ustack();
	ms_ocall_pwrite64_t * ms;

	if (count > 4096) {
		retval = ocall_alloc_untrusted(ALLOC_ALIGNUP(count), &obuf); //  
		if (retval < 0)
			return retval;
	}
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;
	if (obuf) {
		ms->ms_buf = obuf;
		memcpy(obuf, buf, count);
	} else {
		ms->ms_buf = sgx_copy_to_ustack(buf, count);
	}
	ms->ms_count = count;
	ms->ms_offset = offset;

	retval = sgx_ocall(OCALL_PWRITE64, ms);
	sgx_reset_ustack(old_ustack);

	if (obuf)
		ocall_unmap_untrusted(obuf, ALLOC_ALIGNUP(count));

	return retval;
}

int ocall_dup(int fd) {
	int retval = 0;
	ms_ocall_dup_t * ms;
	void * old_ustack = sgx_prepare_ustack();
	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	ms->ms_fd = fd;

	retval = sgx_ocall(OCALL_DUP, ms);
	sgx_reset_ustack(old_ustack);

	return retval;
}
