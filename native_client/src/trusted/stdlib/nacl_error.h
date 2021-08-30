/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

/* Copyright (C) 2014 Stony Brook University
   This file is part of Graphene Library OS.

   Graphene Library OS is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Graphene Library OS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/*
 * nacl_error.h
 *
 * This file contains definitions of NACL error codes.
 */

#ifndef NACL_ERROR_H
#define NACL_ERROR_H

#define NACL_ERROR_NOTIMPLEMENTED        1
#define NACL_ERROR_NOTDEFINED            2
#define NACL_ERROR_NOTSUPPORT            3
#define NACL_ERROR_INVAL                 4
#define NACL_ERROR_TOOLONG               5
#define NACL_ERROR_DENIED                6
#define NACL_ERROR_BADHANDLE             7
#define NACL_ERROR_STREAMEXIST           8
#define NACL_ERROR_STREAMNOTEXIST        9
#define NACL_ERROR_STREAMISFILE         10
#define NACL_ERROR_STREAMISDIR          11
#define NACL_ERROR_STREAMISDEVICE       12
#define NACL_ERROR_INTERRUPTED          13
#define NACL_ERROR_OVERFLOW             14
#define NACL_ERROR_BADADDR              15
#define NACL_ERROR_NOMEM                16
#define NACL_ERROR_NOTKILLABLE          17
#define NACL_ERROR_INCONSIST            18
#define NACL_ERROR_TRYAGAIN             19
#define NACL_ERROR_ENDOFSTREAM          20
#define NACL_ERROR_NOTSERVER            21
#define NACL_ERROR_NOTCONNECTION        22
#define NACL_ERROR_ZEROSIZE             23
#define NACL_ERROR_CONNFAILED           24
#define NACL_ERROR_ADDRNOTEXIST         25

#define NACL_ERROR_BOUND                25

static const char * nacl_errstring[]
#ifdef __GNUC__
__attribute__((unused))
#endif
= {
        /*  0. */ "Success",
        /*  1. */ "Function not implemented",
        /*  2. */ "Symbol not defined",
        /*  3. */ "Function not supported",
        /*  4. */ "Invalid argument",
        /*  5. */ "Name/Path is too long",
        /*  6. */ "Operation Denied",
        /*  7. */ "Handle Corrupted",
        /*  8. */ "Stream already exists",
        /*  9. */ "Stream does not exists",
        /* 10. */ "Stream is File",
        /* 11. */ "Stream is Directory",
        /* 12. */ "Stream is Device",
        /* 13. */ "Operation interrupted",
        /* 14. */ "Buffer overflowed",
        /* 15. */ "Invalid address",
        /* 16. */ "Not enough memory",
        /* 17. */ "Thread state unkillable",
        /* 18. */ "Inconsistent system state",
        /* 19. */ "Try again",
        /* 20. */ "End of stream",
        /* 21. */ "Not a server",
        /* 22. */ "Not a connection",
        /* 23. */ "Zero size",
        /* 24. */ "Connection failed",
        /* 25. */ "Resource address not exist",
    };

static inline const char * NACL_STRERROR (int errnum)
{
    int _e = -errnum;
    if (_e >= 0 && _e <= NACL_ERROR_BOUND)
        return nacl_errstring[_e];
    return "Unknown error";
}

#endif
