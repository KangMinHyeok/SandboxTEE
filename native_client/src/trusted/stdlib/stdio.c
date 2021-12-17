/* Copyright (C) 1993-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.
   As a special exception, if you link the code in this file with
   files compiled with a GNU compiler to produce an executable,
   that does not cause the resulting executable to be covered by
   the GNU Lesser General Public License.  This exception does not
   however invalidate any other reasons why the executable file
   might be covered by the GNU Lesser General Public License.
   This exception applies to code released by its copyright holders
   in files containing the exception.  */
//#include "native_client/src/trusted/stdlib/libioP.h"
//#include "native_client/src/trusted/stdlib/stdfiles.h"
#include "native_client/src/trusted/stdlib/stdio.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1
#undef stdin
#undef stdout
#undef stderr
FILE *stdin = (FILE *) 0;
FILE *stdout = (FILE *) 1;
FILE *stderr = (FILE *) 2;
int open(const char *__file, int __oflag, ...)
{
    short mode = 0;
    va_list ap;
    va_start(ap, __oflag);
    mode = va_arg(ap, int);
    int ret = ocall_open(__file,__oflag, mode);
    va_end(ap);
    return ret;
}
int setvbuf (FILE *__restrict __stream, char *__restrict __buf, int __modes, size_t __n)
{
    //TODO: impl
    int ret = 0;
    return ret;
}

#endif
