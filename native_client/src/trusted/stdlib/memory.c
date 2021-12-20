
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
 * memory.c
 *
 * This file contains implementation of internal memory allocator.
 */

#include "native_client/src/trusted/stdlib/nacl_internal.h"
#include "native_client/src/trusted/stdlib/memheader.h"

#ifndef NO_INTERNAL_ALLOC
#include "native_client/src/trusted/stdlib/list.h"
#include "native_client/src/trusted/stdlib/spinlock.h"
#include <string.h>

#define PRESET_PAGESIZE (1 << 12)
#define PRINT_ENCLAVE_STAT  (0)
#define STATIC_SLAB 1
static int slab_alignment;

#if STATIC_SLAB == 1
# define POOL_SIZE 256 * 1024 * 1024 /* 64MB by default */
static char mem_pool[POOL_SIZE];
static void *bump = mem_pool;
static void *mem_pool_end = &mem_pool[POOL_SIZE];
#else
# define PAGE_SIZE (slab_alignment)
#endif

static inline void * __malloc (int size)
{
    void * addr = NULL;

#if STATIC_SLAB == 1
   if (bump + size <= mem_pool_end) {
        addr = bump;
        bump += size;
        return addr;
    }
#endif

    _DkVirtualMemoryAlloc(&addr, size, NACL_ALLOC_INTERNAL, NACL_PROT_READ | NACL_PROT_WRITE);
    return addr;
}

#define system_malloc(size) __malloc(size)

static inline void __free (void * addr, int size)
{
#if STATIC_SLAB == 1
    if (addr >= (void *)mem_pool && addr < mem_pool_end)
        return;
#endif

    _DkVirtualMemoryFree(addr, size);
}

#define system_free(addr, size) __free(addr, size)

static spinlock_t slab_lock;
#include "slabmgr.h"

static SLAB_MGR slab_mgr = NULL;

void init_slab_mgr (int alignment)
{
    if (slab_mgr)
        return;

    spinlock_init(&slab_lock);
    
    slab_alignment = alignment;
    slab_mgr = create_slab_mgr();
    if (!slab_mgr)
        init_fail(NACL_ERROR_NOMEM, "cannot initialize slab manager");
}

void * malloc (size_t size)
{
    void * ptr = slab_alloc(slab_mgr, size);

#ifdef DEBUG
    /* In debug builds, try to break code that uses uninitialized heap
     * memory by explicitly initializing to a non-zero value. */
    if (ptr)
        memset(ptr, 0xa5, size);
#endif


    if (!ptr) {
        /*
         * Normally, the NACL should not run out of memory.
         * If malloc() failed internally, we cannot handle the
         * condition and must terminate the current process.
         */
        printf("******** Out-of-memory in NACL ********\n");

#if PRINT_ENCLAVE_STAT
        print_alloced_pages();
#endif
        ocall_exit(-1);
    }

    return ptr;
}

void * malloc_copy (const void * mem, size_t size)
{
    void * nmem = malloc(size);

    if (nmem)
        memcpy(nmem, mem, size);

    return nmem;
}

/*char * strdup (const char *s)
{
    size_t len = strlen(s) + 1;
    char *new = malloc(len);

    if (new)
        memcpy(new, s, len);
    
    return new;
}*/

void * calloc (size_t nmem, size_t size)
{
    void * ptr = malloc(nmem * size);

    if (ptr)
        memset(ptr, 0, nmem * size);

    return ptr;
}

void free (void * ptr)
{
    slab_free(slab_mgr, ptr);

}

#endif /* !NO_INTERNAL_ALLOC */
