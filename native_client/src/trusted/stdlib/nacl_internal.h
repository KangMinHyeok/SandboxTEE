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
 * nacl_internal.h
 *
 * This file contains definition of functions, variables and data structures
 * for internal uses.
 */

#ifndef NACL_INTERNAL_H
#define NACL_INTERNAL_H

#include "native_client/src/include/build_config.h"
#include "list.h"

/* (Linux-only) enable caching loaded binaries for optimizing process creation */
#define CACHE_LOADED_BINARIES    0      /* default: disabled */

/* statically allocate slab manager */
#define STATIC_SLAB              1

/* maximum length of URIs */
#define URI_MAX                  256

/* allow binding sockets to ANY addresses (e.g., 0.0.0.0:0) */
#define ALLOW_BIND_ANY           1

/* turn on the following option to trace heap memory leak */
#define TRACE_HEAP_LEAK

#ifdef PROFILING
// #define PROFILING 1
#else
#undef PROFILING
#define PROFILING 0
#endif

/*TODO crhamm
#ifndef IN_ENCLAVE
#error "nacl_internal.h can only be included in NACL"
#endif
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint64_t      NACL_NUM;
typedef const char *  NACL_STR;
typedef void *        NACL_PTR;
typedef uint32_t      NACL_FLG;
typedef uint32_t      NACL_IDX;
typedef bool          NACL_BOL;



#ifdef IN_ENCLAVE
#include "native_client/src/trusted/stdlib/atomic.h"
#else
#endif /* ! IN_ENCLAVE */


//#error "Do not use this header file"



#define NACL_TRUE  true
#define NACL_FALSE false
/* The ABI includes three calls to allocate, free, and modify the
 * permission bits on page-base virtual memory. Permissions in-
 * clude read, write, execute, and guard. Memory regions can be
 * unallocated, reserved, or backed by committed memory
 */

/* Memory Allocation Flags */
#define NACL_ALLOC_RESERVE     0x0001   /* Only reserve the memory */

//TODO crhamm
//#ifdef IN_ENCLAVE
#define NACL_ALLOC_INTERNAL    0x8000
//#endif

/* Memory Protection Flags */
#define NACL_PROT_NONE       0x0     /* 0x0 Page can not be accessed. */
#define NACL_PROT_READ       0x1     /* 0x1 Page can be read. */
#define NACL_PROT_WRITE      0x2     /* 0x2 Page can be written. */
#define NACL_PROT_EXEC       0x4     /* 0x4 Page can be executed. */
#define NACL_PROT_WRITECOPY  0x8     /* 0x8 Copy on write */


extern unsigned long pagesize, pageshift, pagemask;

#define ALLOC_ALIGNDOWN(addr) \
    (pagesize ? ((unsigned long) (addr)) & pagemask : (unsigned long) (addr))
#define ALLOC_ALIGNUP(addr) \
    (pagesize ? (((unsigned long) (addr)) + pageshift) & pagemask : (unsigned long) (addr))
#define ALLOC_ALIGNED(addr) \
    (pagesize && ((unsigned long) (addr)) == (((unsigned long) (addr)) & pagemask))


/* DkVirtualMemory calls */
int _DkVirtualMemoryAlloc (void ** paddr, uint64_t size, int alloc_type, int prot);
int _DkVirtualMemoryFree (void * addr, uint64_t size);

#define init_fail(exitcode, reason)                                     \
    do {                                                                \
        printf("NACL failed at " __FILE__  ":%s:%u (exitcode = %u, reason=%s)\n", \
               __FUNCTION__, (unsigned int)__LINE__,                    \
               (unsigned int) (exitcode), (const char *) (reason));     \
        ocall_exit(exitcode);                                       \
    } while (0)

typedef struct {
    NACL_IDX type;
    NACL_FLG flags;
} NACL_HDR;

DEFINE_LIST(nacl_handle_thread);
struct nacl_handle_thread {
    NACL_HDR reserved;
    NACL_IDX tid;
    NACL_PTR tcs;
    LIST_TYPE(nacl_handle_thread) list;
    void * param;
};
#define HANDLE_TYPE(handle)  ((handle)->hdr.type)

struct arch_frame {
#ifdef __x86_64__
    unsigned long rsp, rbp, rbx, rsi, rdi, r12, r13, r14, r15;
#else
# error "unsupported architecture"
#endif
};

/* function and definition for loading binaries */
enum object_type { OBJECT_RTLD, OBJECT_EXEC, OBJECT_PRELOAD, OBJECT_EXTERNAL };

#ifndef NO_INTERNAL_ALLOC
void init_slab_mgr (int alignment);
void * malloc (size_t size);
void * malloc_copy(const void * mem, size_t size);
void * calloc (size_t nmem, size_t size);
char * strdup(const char *source);
void free (void * mem);
#endif

#ifdef __GNUC__
# define __attribute_hidden __attribute__ ((visibility ("hidden")))
# define __attribute_always_inline __attribute__((always_inline))
# define __attribute_unused __attribute__((unused))
# define __attribute_noinline __attribute__((noinline))
#else
# define __attribute_hidden
# define __attribute_always_inline
# define __attribute_unused
# define __attribute_noinline
#endif

#define alias_str(name) #name
#ifdef __GNUC__
# define extern_alias(name) \
    extern __typeof(name) nacl_##name __attribute ((alias (alias_str(name))))
#else
# define extern_alias(name)
#endif

int printf  (const char  *fmt, ...);
#include <stdarg.h>
// #include <stdio.h>
int vprintf(const char * fmt, va_list *ap);
// int vprintf (const char *__restrict __format, _G_va_list __arg);
void write_log (int nstrs, ...);

#endif
