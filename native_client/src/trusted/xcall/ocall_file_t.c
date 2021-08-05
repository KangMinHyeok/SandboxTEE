#include <string.h>

#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_common.h"


#define MAX_UNTRUSTED_STACK_BUF 128*PRESET_PAGESIZE

int ocall_open (char * pathname, int flags, unsigned short mode) {
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

int ocall_write (int fd, void * buf, unsigned int count) {
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

