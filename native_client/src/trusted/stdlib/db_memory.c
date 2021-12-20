/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */


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
 * db_memory.c
 *
 * This files contains APIs that allocate, free or protect virtual memory.
 */

#include "native_client/src/trusted/stdlib/memheader.h"
#include "native_client/src/trusted/stdlib/nacl_internal.h"
#include "native_client/src/trusted/stdlib/nacl_error.h"
#include "spinlock.h"

#include "native_client/src/trusted/xcall/enclave_framework.h"
#include <asm/mman.h>

#include "enclave_pages.h"
#include "native_client/src/trusted/xcall/ocall_types.h"

#define NACL_VMA_MAX     64


static inline int HOST_PROT (int prot)
{
    return prot & (NACL_PROT_READ | NACL_PROT_WRITE | NACL_PROT_EXEC);
}

#define ACCESS_R    4
#define ACCESS_W    2
#define ACCESS_X    1

struct stat;
bool stataccess (struct stat * stats, int acc);

bool _DkCheckMemoryMappable (const void * addr, int size)
{
    /*
    if (addr < DATA_END && addr + size > TEXT_START) {
        printf("address %p-%p is not mappable\n", addr, addr + size);
        return true;
    }

    _DkSpinLock(&pal_vma_lock);

    for (int i = 0 ; i < pal_nvmas ; i++)
        if (addr < pal_vmas[i].top && addr + size > pal_vmas[i].bottom) {
            printf("address %p-%p is not mappable\n", addr, addr + size);
            _DkSpinUnlock(&pal_vma_lock);
            return true;
        }

    _DkSpinUnlock(&pal_vma_lock);
    */
    return false;
}

int _DkVirtualMemoryAlloc (void ** paddr, uint64_t size, int alloc_type, int prot)
{
    
    void * addr = *paddr;
     void * mem;

    //int flags = HOST_FLAGS(alloc_type, prot|NACL_PROT_WRITECOPY);
    //prot = HOST_PROT(prot);
    /* The memory should have MAP_PRIVATE and MAP_ANONYMOUS */
    //flags |= MAP_ANONYMOUS|(addr ? MAP_FIXED : 0);
    //mem = (void *) ARCH_MMAP(addr, size, prot, flags, -1, 0);

    if ((alloc_type & NACL_ALLOC_INTERNAL) && addr)
        return -NACL_ERROR_INVAL;

    if (size == 0)
        __asm__ volatile ("int $3");

    mem = get_reserved_pages(addr, size);
    if (!mem)
        return addr ? -NACL_ERROR_DENIED : -NACL_ERROR_NOMEM;
    if (addr && mem != addr) {
        // TODO: This case should be made impossible by fixing
        // `get_reserved_pages` semantics.
        free_pages(mem, size);
        return -NACL_ERROR_INVAL; // `addr` was unaligned.
    }

   memset(mem, 0, size);
/*
    if (alloc_type & NACL_ALLOC_INTERNAL) {
        _DkSpinLock(&pal_vma_lock);
        assert(pal_nvmas < NACL_VMA_MAX);
        pal_vmas[pal_nvmas].bottom = mem;
        pal_vmas[pal_nvmas].top = mem + size;
        pal_nvmas++;
        _DkSpinUnlock(&pal_vma_lock);
    }

    *paddr = mem; */
    return 0;
}

int _DkVirtualMemoryFree (void * addr, uint64_t size)
{

    if (sgx_is_completely_within_enclave(addr, size)) {
        free_pages(addr, size);
    } else {
        //printf("OCALL!! free from untrusted mapping\n");
        /* Possible to have untrusted mapping. Simply unmap
           the memory outside the enclave */
        ocall_unmap_untrusted(addr, size);
    }
    return 0;
}

int _DkVirtualMemoryProtect (void * addr, uint64_t size, int prot)
{
    return 0;
}
/*
unsigned long _DkMemoryQuota (void)
{
    return pal_sec.heap_max - pal_sec.heap_min;
}

extern struct atomic_int alloced_pages;
extern unsigned int pagesz;

unsigned long _DkMemoryAvailableQuota (void)
{
    return (pal_sec.heap_max - pal_sec.heap_min) -
        atomic_read(&alloced_pages) * pagesz;
}
*/
