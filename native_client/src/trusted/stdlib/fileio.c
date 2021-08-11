/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "native_client/src/trusted/stdlib/fileio.h"
#include "native_client/src/trusted/stdlib/filestruct.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/include/build_config.h"
#include <sys/stat.h>
#if NACL_SGX == 1

int fflush(FILE *fp)
{
    int ret = 0;//ocall_fflush(fp);
    return ret;
}

int fsync(int fd)
{
    int ret = ocall_fsync(fd);
    return ret;
}

int ferror(FILE *fp)
{
    int ret = 0;//ocall_ferror(fp);
    return ret;
}

long ftell(FILE *fp)
{
    long ret = 0;//ocall_ftell(fp);
    return ret;
}

int fseek(FILE *fp, long offset, int whence)
{
    int ret = 0;//ocall_fseek(fp, offset, whence);
    return ret;
}

FILE * fopen(const char *file, const char *mode)
{
    FILE * fp = 0;//ocall_fopen(file, mode);
    return fp;
}

int fclose(FILE *fp)
{
    int ret = 0;//ocall_fclose(fp);
    return ret;
}

size_t fread(void * buf, size_t size, size_t count, register FILE *fp)
{
    size_t ret = 0;//ocall_fread(buf, size, count, fp);
    return ret;
}

size_t fwrite(const void *buf, size_t size, size_t count, register FILE *fp)
{
    size_t ret = 0;//ocall_fwrite(buf, size, count, fp);
    return ret;
}

int flock(int fd, int operation)
{
    int ret = 0;//ocall_flock(fd, operation);
    return ret;
}

int fchmod(int fd, __mode_t mode)
{
    int ret = ocall_fchmod(fd, (unsigned short) mode);
    return ret;
}

int fcntl(int fd, int cmd, ...)
{
	// TODO: fix args
	long args = 0;
    int ret = ocall_fcntl(fd, cmd, args);
    return ret;
}

int fdatasync(int fd)
{
    int ret = ocall_fdatasync(fd);
    return ret;
}

FILE * fdopen(int fd, const char *mode)
{
    FILE *fp = 0;//ocall_fdopen(fd, mode);
    return fp;
}

int ffs(register int mask)
{
  register int bit;
  if (mask == 0)
    return (0);
  for (bit = 1; !(mask & 1); bit++)
    mask >>= 1;
  return (bit);
}

int fileno(FILE *fp)
{
	if (fp->_fileno < 0)
		return -1;
	return fp->_fileno;
}

int fstat64(int fd, struct stat64 *buf)
{   
    // TODO: impl
    return 0;
}

void perror(const char *s)
{
    // TODO: impl
    return;
}

int getpid(void)
{
    // TODO: impl
    int ret = ocall_getpid();
    return ret;
}

int dup(int oldfd)
{
    // TODO: impl
    return oldfd;
}
#endif
