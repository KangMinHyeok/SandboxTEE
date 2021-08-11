/* Copyright (C) 1991-2019 Free Software Foundation, Inc.
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
   <http://www.gnu.org/licenses/>.  */

#ifndef _ERRNO_H_
#define _ERRNO_H_

#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1

/* The error code set by various library functions.  */
extern int *__errno_location (void);// __THROW __attribute_const__;
# define errno (*__errno_location ())

# define __set_errno(val) (errno = (val))


#define        EPERM                 1        /* Operation not permitted */
#define        ENOENT                 2        /* No such file or directory */
#define        ESRCH                 3        /* No such process */
#define        EINTR                 4        /* Interrupted system call */
#define        EIO                 5        /* I/O error */
#define        ENXIO                 6        /* No such device or address */
#define        E2BIG                 7        /* Argument list too long */
#define        ENOEXEC                 8        /* Exec format error */
#define        EBADF                 9        /* Bad file number */
#define        ECHILD                10        /* No child processes */
#define        EAGAIN                11        /* Try again */
#define        ENOMEM                12        /* Out of memory */
#define        EACCES                13        /* Permission denied */
#define        EFAULT                14        /* Bad address */
#define        ENOTBLK                15        /* Block device required */
#define        EBUSY                16        /* Device or resource busy */
#define        EEXIST                17        /* File exists */
#define        EXDEV                18        /* Cross-device link */
#define        ENODEV                19        /* No such device */
#define        ENOTDIR                20        /* Not a directory */
#define        EISDIR                21        /* Is a directory */
#define        EINVAL                22        /* Invalid argument */
#define        ENFILE                23        /* File table overflow */
#define        EMFILE                24        /* Too many open files */
#define        ENOTTY                25        /* Not a typewriter */
#define        ETXTBSY                26        /* Text file busy */
#define        EFBIG                27        /* File too large */
#define        ENOSPC                28        /* No space left on device */
#define        ESPIPE                29        /* Illegal seek */
#define        EROFS                30        /* Read-only file system */
#define        EMLINK                31        /* Too many links */
#define        EPIPE                32        /* Broken pipe */
#define        EDOM                33        /* Math argument out of domain of func */
#define        ERANGE                34        /* Math result not representable */


#endif

#endif
