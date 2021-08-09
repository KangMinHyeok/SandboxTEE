#include "native_client/src/trusted/stdlib/memory.h"

#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/ocall_types.h"

#include <stdio.h>

static const int slab_levels[SLAB_LEVEL] = { SLAB_LEVEL_SIZES };
static spinlock_t slab_lock;
static SLAB_MGR slab_mgr = NULL;

#define system_malloc(size) __malloc(size)
#define system_lock() spinlock_lock(&slab_lock)
#define system_unlock() spinlock_unlock(&slab_lock)
#define system_free(addr, size) __free(addr, size)


#if STATIC_SLAB == 1
# define POOL_SIZE 256 * 1024 * 1024 /* 64MB by default */
static char mem_pool[POOL_SIZE];
static void *bump = mem_pool;
static void *mem_pool_end = &mem_pool[POOL_SIZE];
#else
#define PAGE_SIZE (slab_alignment)
#endif

static unsigned long pgsz = PRESET_PAGESIZE;
void * heap_base;
static uint64_t heap_size;

DEFINE_LIST(heap_vma);
struct heap_vma {
    LIST_TYPE(heap_vma) list;
    void * top;
    void * bottom;
};

DEFINE_LISTP(heap_vma);
static LISTP_TYPE(heap_vma) heap_vma_list = LISTP_INIT;
spinlock_t heap_vma_lock;

struct atomic_int alloced_pages, max_alloced_pages;

static void assert_vma_list (void)
{
#if ASSERT_VMA == 1
    void * last_addr = heap_base + heap_size;
    struct heap_vma * vma;

    listp_for_each_entry(vma, &heap_vma_list, list) {
        printf(   "[%d] %p - %p\n", pal_sec.pid, vma->bottom, vma->top);
        if (last_addr < vma->top || vma->top <= vma->bottom) {
            printf( "*** [%d] corrupted heap vma: %p - %p (last = %p) ***\n", pal_sec.pid, vma->bottom, vma->top, last_addr);
#ifdef DEBUG
            if (pal_sec.in_gdb)
                __asm__ volatile ("int $3" ::: "memory");
#endif
            ocall_exit();
        }
        last_addr = vma->bottom;
    }
#endif
}

void * get_reserved_pages(void * addr, uint64_t size)
{
    if (!size)
        return NULL;

    printf("*** get_reserved_pages: heap_base %p, heap_size %llu, limit %p ***\n", heap_base, heap_size, heap_base + heap_size);
    if (addr >= heap_base + heap_size) {
        printf(   "*** allocating out of heap: %p ***\n", addr);
        return NULL;
    }

    if (size & (pgsz - 1))
        size = ((size + pgsz - 1) & ~(pgsz - 1));

    if ((uintptr_t) addr & (pgsz - 1))
        addr = (void *) ((uintptr_t) addr & ~(pgsz - 1));
        
    printf("allocate %d bytes at %p\n", size, addr);

    spinlock_lock(&heap_vma_lock);

    struct heap_vma * prev = NULL, * next;
    struct heap_vma * vma;

    /* Allocating in the heap region.  This loop searches the vma list to
     * find the first vma with a starting address lower than the requested
     * address.  Recall that vmas are in descending order.
     *
     * If the very first vma matches, prev will be null.
     */
    if (addr && addr >= heap_base &&
        addr + size <= heap_base + heap_size) {
        listp_for_each_entry(vma, &heap_vma_list, list) {
            if (vma->bottom < addr)
                break;
                 prev = vma;
        }
        goto allocated;
    }

    if (addr) {
        spinlock_unlock(&heap_vma_lock);
        return NULL;
    }

    void * avail_top = heap_base + heap_size;

    listp_for_each_entry(vma, &heap_vma_list, list) {
        if ((unsigned) (avail_top - vma->top) > size) {
            addr = avail_top - size;
            goto allocated;
        }
        prev = vma;
         avail_top = prev->bottom;
    }

    if (avail_top >= heap_base + size) {
        addr = avail_top - size;
        goto allocated;
    }

    spinlock_unlock(&heap_vma_lock);

    printf(    "*** Not enough space on the heap (requested = %llu) ***\n", size);
    __asm__ volatile("int $3");
    return NULL;

allocated:
    if (prev) {
        // If this is the last entry, don't wrap around
        if (prev->list.next == listp_first_entry(&heap_vma_list, struct heap_vma, list))

        next = NULL;
        else
            next = prev->list.next;
    } else {
        /* In this case, the list is empty, or
         * first vma starts at or below the allocation site.
         *
         * The next field will be used to merge vmas with the allocation, if
         * they overlap, until the vmas drop below the requested addr
         * (traversing in decreasing virtual address order)
         */
        next = listp_empty(&heap_vma_list) ? NULL :
            listp_first_entry(&heap_vma_list, struct heap_vma, list);
    }

    if (prev && next)
        printf(   "insert vma between %p-%p and %p-%p\n",
                next->bottom, next->top, prev->bottom, prev->top);
                else if (prev)
        printf(   "insert vma below %p-%p\n", prev->bottom, prev->top);
    else if (next)
        printf(   "insert vma above %p-%p\n", next->bottom, next->top);

    vma = NULL;
    while (prev) {
         struct heap_vma * prev_prev = NULL;

        if (prev->bottom > addr + size)
            break;

        /* This appears to be doing a reverse search; we should stop before we
         * wrap back to the last entry */
        if (prev->list.prev != listp_last_entry(&heap_vma_list, struct heap_vma, list))
            prev_prev = list_entry(prev->list.prev, struct heap_vma, list);

        if (!vma) {
         printf(   "merge %p-%p and %p-%p\n", addr, addr + size,
                    prev->bottom, prev->top);

            vma = prev;
            vma->top = (addr + size > vma->top) ? addr + size : vma->top;
            vma->bottom = addr;
        } else {
            printf(   "merge %p-%p and %p-%p\n", vma->bottom, vma->top,
                    prev->bottom, prev->top);

            vma->top = (prev->top > vma->top) ? prev->top : vma->top;
            listp_del(prev, &heap_vma_list,list);
            free(prev);
        }

        prev = prev_prev;
    }
 while (next) {
        struct heap_vma * next_next = NULL;

        if (next->top < addr)
            break;

        if (next->list.next != listp_first_entry(&heap_vma_list, struct heap_vma, list))
            next_next = list_entry(next->list.next, struct heap_vma, list);

        if (!vma) {
            printf(   "merge %p-%p and %p-%p\n", addr, addr + size,
                    next->bottom, next->top);

            vma = next;
            vma->top = (addr + size > vma->top) ? addr + size : vma->top;
        } else {
            printf(   "merge %p-%p and %p-%p\n", vma->bottom, vma->top,
                    next->bottom, next->top);
                     vma->bottom = next->bottom;
                      listp_del(next, &heap_vma_list, list);
            free(next);
        }

        next = next_next;
    }

    if (!vma) {
        vma = malloc(sizeof(struct heap_vma));
        if (!vma) {
            spinlock_unlock(&heap_vma_lock);
            return NULL;
        }
        vma->top = addr + size;
        vma->bottom = addr;
        INIT_LIST_HEAD(vma, list);
        listp_add_after(vma, prev, &heap_vma_list, list);
        }

    if (vma->bottom >= vma->top) {
        printf( "*** Bad memory bookkeeping: %p - %p ***\n",
                vma->bottom, vma->top);
#ifdef DEBUG
        /*
        if (pal_sec.in_gdb)
            __asm__ volatile ("int $3" ::: "memory"); */
#endif
    }
    assert_vma_list();

    spinlock_unlock(&heap_vma_lock);

    atomic_add(size / pgsz, &alloced_pages);
    return addr;
}

void free_pages(void * addr, uint64_t size)
{
    void * addr_top = addr + size;

    printf(   "free_pages: trying to free %p %llu\n", addr, size);

    if (!addr || !size)
        return;

    if ((uintptr_t) addr_top & (pgsz - 1))
        addr = (void *) (((uintptr_t) addr_top + pgsz + 1) & ~(pgsz - 1));

    if ((uintptr_t) addr & (pgsz - 1))
        addr = (void *) ((uintptr_t) addr & ~(pgsz - 1));

    if (addr >= heap_base + heap_size)
        return;
    if (addr_top <= heap_base)
        return;
    if (addr_top > heap_base + heap_size)
        addr_top = heap_base + heap_size;
    if (addr < heap_base)
        addr = heap_base;

    printf(   "free %d bytes at %p\n", size, addr);

    spinlock_lock(&heap_vma_lock);

    struct heap_vma * vma, * p;

    listp_for_each_entry_safe(vma, p, &heap_vma_list, list) {
        if (vma->bottom >= addr_top)
            continue;
        if (vma->top <= addr)
            break;
        if (vma->bottom < addr) {
            struct heap_vma * new = malloc(sizeof(struct heap_vma));
            new->top = addr;
            new->bottom = vma->bottom;
            INIT_LIST_HEAD(new, list);
            list_add(new, vma, list);
        }

        vma->bottom = addr_top;
        if (vma->top <= vma->bottom) {
            listp_del(vma, &heap_vma_list, list); free(vma);
        }
    }

    assert_vma_list();

    spinlock_unlock(&heap_vma_lock);

    unsigned int val = atomic_read(&alloced_pages);
    atomic_sub(size / pgsz, &alloced_pages);
    if (val > atomic_read(&max_alloced_pages))
        atomic_set(&max_alloced_pages, val);
}

int _DkVirtualMemoryAlloc (void ** paddr, uint64_t size, int alloc_type, int prot)
{

    void * addr = *paddr, * mem;

    //int flags = HOST_FLAGS(alloc_type, prot|PAL_PROT_WRITECOPY);
    //prot = HOST_PROT(prot);
    /* The memory should have MAP_PRIVATE and MAP_ANONYMOUS */
    //flags |= MAP_ANONYMOUS|(addr ? MAP_FIXED : 0);
    //mem = (void *) ARCH_MMAP(addr, size, prot, flags, -1, 0);
     if ((alloc_type & PAL_ALLOC_INTERNAL) && addr)
        return -PAL_ERROR_INVAL;

    if (size == 0)
        __asm__ volatile ("int $3");

    mem = get_reserved_pages(addr, size);
    if (!mem)
        return addr ? -PAL_ERROR_DENIED : -PAL_ERROR_NOMEM;
    if (addr && mem != addr) {
        // TODO: This case should be made impossible by fixing
        // `get_reserved_pages` semantics.
        free_pages(mem, size);
        return -PAL_ERROR_INVAL; // `addr` was unaligned.
    }

    memset(mem, 0, size);
/*
    if (alloc_type & PAL_ALLOC_INTERNAL) {
        SGX_DBG(DBG_M, "pal allocates %p-%p for internal use\n", mem, mem + size);
        _DkSpinLock(&pal_vma_lock);
        assert(pal_nvmas < PAL_VMA_MAX);
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
        printf("OCALL!! free from untrusted mapping\n");
        /* Possible to have untrusted mapping. Simply unmap
           the memory outside the enclave */
        ocall_unmap_untrusted(addr, size);
    }
    return 0;
}


/* This function is protected by slab_mgr_lock. */
static inline void * __malloc (int size)
{
    void * addr = NULL;

#if STATIC_SLAB == 1
    // debug_print("%s [%d]: %d\n", __FUNCTION__, __LINE__, size);
    if (bump + size <= mem_pool_end) {
        addr = bump;
        bump += size;
        return addr;
    }
#endif

    // TODO FIX here..
    //int ret;
    //ret = 
    _DkVirtualMemoryAlloc(&addr, size, PAL_ALLOC_INTERNAL,
                          PAL_PROT_READ|PAL_PROT_WRITE);
    return addr;
}

static inline void __set_free_slab_area (SLAB_AREA area, SLAB_MGR mgr,
        int level)
{
    int slab_size = slab_levels[level] + SLAB_HDR_SIZE;
    mgr->addr[level] = (void *) area->raw;
    mgr->addr_top[level] = (void *) area->raw + (area->size * slab_size);
    mgr->size[level] += area->size;
    mgr->active_area[level] = area;
}

static inline int enlarge_slab_mgr (SLAB_MGR mgr, int level)
{
    assert(level < SLAB_LEVEL);
    /* DEP 11/24/17: This strategy basically doubles a level's size
     * every time it grows.  The assumption if we get this far is that
     * mgr->addr == mgr->top_addr */
    assert(mgr->addr[level] == mgr->addr_top[level]);

    size_t size = mgr->size[level];
    SLAB_AREA area;

    /* If there is a previously allocated area, just activate it. */
    area = listp_prev_entry(mgr->active_area[level], &mgr->area_list[level], __list);
    if (area) {
        __set_free_slab_area(area, mgr, level);
        return 0;
    }
    /* system_malloc() may be blocking, so we release the lock before
     * allocating more memory */
    system_unlock();

    /* If the allocation failed, always try smaller sizes */
    for (; size > 0; size >>= 1) {
        area = (SLAB_AREA) system_malloc(__MAX_MEM_SIZE(slab_levels[level], size));
        if (area)
            break;
    }

    if (!area) {
        system_lock();
        return -ENOMEM;
    }

    system_lock();

area->size = size;
    INIT_LIST_HEAD(area, __list);

    /* There can be concurrent operations to extend the SLAB manager. In case
     * someone has already enlarged the space, we just add the new area to the
     * list for later use. */
    listp_add(area, &mgr->area_list[level], __list);
    if (mgr->size[level] == size) /* check if the size has changed */
        __set_free_slab_area(area, mgr, level);

    return 0;
}

static inline void * slab_alloc (SLAB_MGR mgr, int size)
{
    SLAB_OBJ mobj;
    int i;
    int level = -1;

    for (i = 0 ; i < SLAB_LEVEL ; i++)
        if (size <= slab_levels[i]) {
            level = i;
            break;
        }

    if (level == -1) {
        LARGE_MEM_OBJ mem = (LARGE_MEM_OBJ)
            system_malloc(sizeof(LARGE_MEM_OBJ_TYPE) + size);
        if (!mem)
            return NULL;

        mem->size = size;
        OBJ_LEVEL(mem) = (unsigned char) -1;

        return OBJ_RAW(mem);
    }
    system_lock();
    assert(mgr->addr[level] <= mgr->addr_top[level]);
    if (mgr->addr[level] == mgr->addr_top[level] &&
            listp_empty(&mgr->free_list[level])) {
        int ret = enlarge_slab_mgr(mgr, level);
        if (ret < 0) {
            system_unlock();
            return NULL;
        }
    }

    if (!listp_empty(&mgr->free_list[level])) {
        mobj = listp_first_entry(&mgr->free_list[level], SLAB_OBJ_TYPE, __list);
        listp_del(mobj, &mgr->free_list[level], __list);
    } else {
        mobj = (void *) mgr->addr[level];
        mgr->addr[level] += slab_levels[level] + SLAB_HDR_SIZE;
    }
    assert(mgr->addr[level] <= mgr->addr_top[level]);
    OBJ_LEVEL(mobj) = level;
    system_unlock();

#ifdef SLAB_CANARY
    unsigned long * m =
        (unsigned long *) ((void *) OBJ_RAW(mobj) + slab_levels[level]);
    *m = SLAB_CANARY_STRING;
#endif

    return OBJ_RAW(mobj);
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
         * Normally, the PAL should not run out of memory.
         * If malloc() failed internally, we cannot handle the
         * condition and must terminate the current process.
         */
        printf("******** Out-of-memory in PAL ********\n");

#if PRINT_ENCLAVE_STAT
        print_alloced_pages();
#endif
         // SGX_DBG(DBG_I, "DkProcessExit: Returning exit code %d\n", exitcode);
        
        //TODO
        //ocall_exit(-1);
    }

    return ptr;
}

size_t getsize(void * p) {
        size_t * in = p;
                if (in) { --in; return *in; }
                        return -1;
}

void* realloc (void *ptr, size_t size)
{
    void *res;

  printf("------buggy realloc is called...\n");

    if (!ptr) { // no existing malloc
        res = malloc(size);
        goto realloc_out;
    }
    size_t msize = getsize(ptr);
    if((int)size < (int)msize){
        res = ptr;
    }
    else{
        res = malloc(size);
        if (msize > 0) {
            memcpy(res, ptr, msize);
            free(ptr);
        }
    }

realloc_out:
    return res;
}

void * memcpy (void *dstpp, const void *srcpp, size_t len)
{
    unsigned long int dstp = (long int) dstpp;
    unsigned long int srcp = (long int) srcpp;

    /* Copy from the beginning to the end.  */

    /* If there not too few bytes to copy, use word copy.  */
    if (len >= OP_T_THRES) {
        /* Copy just a few bytes to make DSTP aligned.  */
        len -= (-dstp) % OPSIZ;
        BYTE_COPY_FWD (dstp, srcp, (-dstp) % OPSIZ);

        /* Copy from SRCP to DSTP taking advantage of the known alignment of
             DSTP.  Number of bytes remaining is put in the third argument,
             i.e. in LEN.  This number may vary from machine to machine.  */
        WORD_COPY_FWD (dstp, srcp, len, len);

        /* Fall out and copy the tail.  */
    }

    /* There are just a few bytes to copy.  Use byte memory operations.  */
    BYTE_COPY_FWD (dstp, srcp, len);

    return dstpp;
}

void __free (void * addr, int size)
{
#if STATIC_SLAB == 1
    if (addr >= (void *)mem_pool && addr < mem_pool_end)
        return;
#endif

    _DkVirtualMemoryFree(addr, size);
}

/* This does not return */

//TODO
void __abort(void) {
    //ocall_exit(13);
}

static inline void slab_free (SLAB_MGR mgr, void * obj)
{
/* In a general purpose allocator, free of NULL is allowed (and is a
     * nop). We might want to enforce stricter rules for our allocator if
     * we're sure that no clients rely on being able to free NULL. */
    if (!obj)
        return;

    unsigned char level = RAW_TO_LEVEL(obj);

    if (level == (unsigned char) -1) {
        LARGE_MEM_OBJ mem = RAW_TO_OBJ(obj, LARGE_MEM_OBJ_TYPE);
        system_free(mem, mem->size + sizeof(LARGE_MEM_OBJ_TYPE));
        return;
    }

    /* If this happens, either the heap is already corrupted, or someone's
     * freeing something that's wrong, which will most likely lead to heap
     * corruption. Either way, panic if this happens. TODO: this doesn't allow
     * us to detect cases where the heap headers have been zeroed, which
     * is a common type of heap corruption. We could make this case slightly
     * more likely to be detected by adding a non-zero offset to the level,
     * so a level of 0 in the header would no longer be a valid level. */
    if (level >= SLAB_LEVEL) {
        printf("Heap corruption detected: invalid heap level %d\n", level);
        __abort();
    }

#ifdef SLAB_CANARY
    unsigned long * m = (unsigned long *) (obj + slab_levels[level]);
    assert((*m) == SLAB_CANARY_STRING);
#endif

    SLAB_OBJ mobj = RAW_TO_OBJ(obj, SLAB_OBJ_TYPE);

    system_lock();
    INIT_LIST_HEAD(mobj, __list);
    listp_add_tail(mobj, &mgr->free_list[level], __list);
    system_unlock();
}

void free (void * ptr)
{
    slab_free(slab_mgr, ptr);

}

void * calloc (size_t nmem, size_t size)
{
    void * ptr = malloc(nmem * size);

    if (ptr)
        memset(ptr, 0, nmem * size);

    return ptr;
}

void * memmove (void * destpp, const void * srcpp, size_t len)
{
        unsigned long int dstp = (long int) destpp;
        unsigned long int srcp = (long int) srcpp;

        /* This test makes the forward copying code be used whenever possible.
                 Reduces the working set.  */
        if (dstp - srcp >= len) { /* *Unsigned* compare!  */
                /* Copy from the beginning to the end.  */

                /* If there not too few bytes to copy, use word copy.  */
                if (len >= OP_T_THRES) {
                        /* Copy just a few bytes to make DSTP aligned.  */
                        len -= (-dstp) % OPSIZ;
                        BYTE_COPY_FWD (dstp, srcp, (-dstp) % OPSIZ);

                        /* Copy from SRCP to DSTP taking advantage of the known
                                 alignment of DSTP.  Number of bytes remaining is put
                                 in the third argument, i.e. in LEN.  This number may
                                 vary from machine to machine.  */
                        WORD_COPY_FWD (dstp, srcp, len, len);

                        /* Fall out and copy the tail.  */
                }

                /* There are just a few bytes to copy.  Use byte memory operations.  */
                BYTE_COPY_FWD (dstp, srcp, len);
        } else {
          /* Copy from the end to the beginning.  */
                srcp += len;
                dstp += len;

                /* If there not too few bytes to copy, use word copy.  */
                if (len >= OP_T_THRES) {
                        /* Copy just a few bytes to make DSTP aligned.  */
                        len -= dstp % OPSIZ;
                        BYTE_COPY_BWD (dstp, srcp, dstp % OPSIZ);

                        /* Copy from SRCP to DSTP taking advantage of the known
                         alignment of DSTP.  Number of bytes remaining is put
                                 in the third argument, i.e. in LEN.  This number may
                                 vary from machine to machine.  */
                        WORD_COPY_BWD (dstp, srcp, len, len);

                        /* Fall out and copy the tail.  */
                }

                /* There are just a few bytes to copy.  Use byte memory operations.  */
                BYTE_COPY_BWD (dstp, srcp, len);
        }

        return destpp;
}

void * memset (void *dstpp, int c, size_t len)
{
  long int dstp = (long int) dstpp;

  if (len >= 8)
    {
      int xlen;
      op_t cccc;

      cccc = (unsigned char) c;
      cccc |= cccc << 8;
      cccc |= cccc << 16;
      if (OPSIZ > 4)
    /* Do the shift in two steps to avoid warning if long has 32 bits.  */
    cccc |= (cccc << 16) << 16;

      /* There are at least some bytes to set.
     No need to test for LEN == 0 in this alignment loop.  */
      while (dstp % OPSIZ != 0)
    {
      ((byte *) dstp)[0] = c;
      dstp += 1;
      len -= 1;
    }

      /* Write 8 `op_t' per iteration until less than 8 `op_t' remain.  */
 xlen = len / (OPSIZ * 8);
      while (xlen > 0)
    {
      ((op_t *) dstp)[0] = cccc;
      ((op_t *) dstp)[1] = cccc;
      ((op_t *) dstp)[2] = cccc;
      ((op_t *) dstp)[3] = cccc;
      ((op_t *) dstp)[4] = cccc;
      ((op_t *) dstp)[5] = cccc;
      ((op_t *) dstp)[6] = cccc;
      ((op_t *) dstp)[7] = cccc;
      dstp += 8 * OPSIZ;
      xlen -= 1;
    }
      len %= OPSIZ * 8;

      /* Write 1 `op_t' per iteration until less than OPSIZ bytes remain.  */
      xlen = len / OPSIZ;
      while (xlen > 0)
    {
      ((op_t *) dstp)[0] = cccc;
      dstp += OPSIZ;
      xlen -= 1;
    }
      len %= OPSIZ;
    }

  /* Write the last few bytes.  */
  while (len > 0)
    {
      ((byte *) dstp)[0] = c;
      dstp += 1;
      len -= 1;
    }

  return dstpp;
}
