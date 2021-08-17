
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
 * slab.c
 *
 * This file contains implementation of PAL's internal memory allocator.
 */

#include "pal_internal.h"
#include "api.h"

//#ifndef NO_INTERNAL_ALLOC
#include "list.h"
#include "pal_defs.h"
#include "pal_error.h"

#include "spinlock.h"
#include <string.h>

//#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#define PRESET_PAGESIZE (1 << 12)
#define PRINT_ENCLAVE_STAT  (0)
#define STATIC_SLAB 1
static int slab_alignment;
//static spinlock_t slab_mgr_lock;

#if STATIC_SLAB == 1
# define POOL_SIZE 256 * 1024 * 1024 /* 64MB by default */
static char mem_pool[POOL_SIZE];
static void *bump = mem_pool;
static void *mem_pool_end = &mem_pool[POOL_SIZE];
#else
# define PAGE_SIZE (slab_alignment)
#endif

//#define STARTUP_SIZE   1

/* This function is protected by slab_mgr_lock. */
static inline void * __malloc (int size)
{
    void * addr = NULL;
    //TODO crhamm 
    ocall_debugp(POOL_SIZE);
    ocall_debugp((long int) bump);
    ocall_debugp((long int) mem_pool_end);

#if STATIC_SLAB == 1
    // debug_print("%s [%d]: %d\n", __FUNCTION__, __LINE__, size);
    if(size <= POOL_SIZE) {
   // TODO crhamm if (bump + size <= mem_pool_end) {
        addr = bump;
        bump += size;
        ocall_debugp((long int) bump);
        return addr;
    }
#endif

    // TODO FIX here..
    //TODO crhamm int ret;
    //ret = 
    _DkVirtualMemoryAlloc(&addr, size, PAL_ALLOC_INTERNAL,
                          PAL_PROT_READ|PAL_PROT_WRITE);
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
    //slab_mgr = NULL;

    if (slab_mgr)
        return;

    spinlock_init(&slab_lock);
    
    slab_alignment = alignment;
    ocall_debugp(alignment);
    slab_mgr = create_slab_mgr();
    ocall_debugp(513);
    if (!slab_mgr)
        init_fail(PAL_ERROR_NOMEM, "cannot initialize slab manager");
}

void * malloc (size_t size)
{
    ocall_debugp(500);
    void * ptr = slab_alloc(slab_mgr, size);
    ocall_debugp(501);
#ifdef DEBUG
    /* In debug builds, try to break code that uses uninitialized heap
     * memory by explicitly initializing to a non-zero value. */
    if (ptr)
        memset(ptr, 0xa5, size);
#endif

/*
    if (!ptr) {*/
        /*
         * Normally, the PAL should not run out of memory.
         * If malloc() failed internally, we cannot handle the
         * condition and must terminate the current process.
         */
        //TODO crhamm printf("******** Out-of-memory in PAL ********\n");
/*
#if PRINT_ENCLAVE_STAT
        print_alloced_pages();
#endif
            // SGX_DBG(DBG_I, "DkProcessExit: Returning exit code %d\n", exitcode);
        //TODO crhamm ocall_exit(-1);
    }
*/
    return ptr;
}

// Copies data from `mem` to a newly allocated buffer of a specified size.
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

//#endif /* !NO_INTERNAL_ALLOC */
