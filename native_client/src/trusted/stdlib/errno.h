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


#endif

#endif
