#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "assert.h"
#include <stddef.h>
#include <stdint.h>
#include <asm/fcntl.h>
#include <errno.h>
#include <stdio.h>
//#include "pal_internal.h"

#include "list.h"
#include "spinlock.h"
#include "pal_error.h"
#include "atomic.h"

#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/xcall/enclave_exception.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"

void *memalign(size_t align, size_t len);

//extern void __assert_fail (__const char *__assertion, __const char *__file,
//               unsigned int __line, __const char *__function)
//     __THROW __attribute__ ((__noreturn__));

int ocall_unmap_untrusted (const void * mem, uint64_t size);

void * memcpy (void *dstpp, const void *srcpp, size_t len);
void * memmove (void *dstpp, const void *srcpp, size_t len);
void * memset (void *dstpp, int c, size_t len);
int memcmp (const void *s1, const void *s2, size_t len);

int _DkVirtualMemoryAlloc (void ** paddr, uint64_t size, int alloc_type, int prot);
int _DkVirtualMemoryFree (void * addr, uint64_t size);
void init_slab_mgr (int alignment);

void *malloc(size_t size);
void *realloc (void *ptr, size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);

#define init_fail(exitcode, reason)                                     \
    do {                                                                \
        printf("PAL failed at " __FILE__  ":%s:%u (exitcode = %u, reason=%s)\n", \
               __FUNCTION__, (unsigned int)__LINE__,                    \
               (unsigned int) (exitcode), (const char *) (reason));     \
        ocall_exit(exitcode);                                       \
    } while (0)

#define PAL_ALLOC_INTERNAL    0x8000
#define PRESET_PAGESIZE (1 << 12)
#define ASSERT_VMA 0
#define STARTUP_SIZE 16

/* Memory Protection Flags */
#define PAL_PROT_NONE       0x0     /* 0x0 Page can not be accessed. */
#define PAL_PROT_READ       0x1     /* 0x1 Page can be read. */
#define PAL_PROT_WRITE      0x2     /* 0x2 Page can be written. */
#define PAL_PROT_EXEC       0x4     /* 0x4 Page can be executed. */
#define PAL_PROT_WRITECOPY  0x8     /* 0x8 Copy on write */

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
# define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif



//stdlib/memcopy.h
#define BYTE_COPY_FWD(dst_bp, src_bp, nbytes)                                 \
  do                                                                          \
    {                                                                         \
      int __nbytes = (nbytes);                                        \
      while (__nbytes > 0)                                                    \
        {                                                                     \
          byte __x = ((byte *) src_bp)[0];                                    \
          src_bp += 1;                                                        \
          __nbytes -= 1;                                                      \
          ((byte *) dst_bp)[0] = __x;                                         \
          dst_bp += 1;                                                        \
        }                                                                     \
    } while (0)

#define BYTE_COPY_BWD(dst_ep, src_ep, nbytes)                                 \
  do                                                                          \
    {                                                                         \
      int __nbytes = (nbytes);                                        \
      while (__nbytes > 0)                                                    \
        {                                                                     \
          byte __x;                                                           \
          src_ep -= 1;                                                        \
          __x = ((byte *) src_ep)[0];                                         \
          dst_ep -= 1;                                                        \
          __nbytes -= 1;                                                      \
          ((byte *) dst_ep)[0] = __x;                                         \
        }                                                                     \
    } while (0)

extern void _wordcopy_bwd_aligned (long int, long int, int);
extern void _wordcopy_bwd_dest_aligned (long int, long int, int);
#define WORD_COPY_BWD(dst_ep, src_ep, nbytes_left, nbytes)                    \
  do                                                                          \
    {                                                                         \
      if (src_ep % OPSIZ == 0)                                                \
        _wordcopy_bwd_aligned (dst_ep, src_ep, (nbytes) / OPSIZ);             \
      else                                                                    \
        _wordcopy_bwd_dest_aligned (dst_ep, src_ep, (nbytes) / OPSIZ);        \
      src_ep -= (nbytes) & -OPSIZ;                                            \
      dst_ep -= (nbytes) & -OPSIZ;                                            \
      (nbytes_left) = (nbytes) % OPSIZ;                                       \
    } while (0)

//stdlib/slabmgr.h
#define OBJ_PADDING   15
#define LARGE_OBJ_PADDING 8

#define RAW_TO_LEVEL(raw_ptr) \
    (*((const unsigned char *) (raw_ptr) - OBJ_PADDING - 1))
#define RAW_TO_OBJ(raw_ptr, type) container_of((raw_ptr), type, raw)
typedef struct __attribute__((packed)) large_mem_obj {
    // offset 0
    unsigned long size;  // User buffer size (i.e. excluding control structures)
    unsigned char large_padding[LARGE_OBJ_PADDING];
    // offset 16
    unsigned char level;
    unsigned char padding[OBJ_PADDING];
    // offset 32 - crhamm
    unsigned char raw[32];
} LARGE_MEM_OBJ_TYPE, * LARGE_MEM_OBJ;

//stdlib/sysdeps/generic/memcopy.h
#define OP_T_THRES  16
#define op_t    unsigned long int
#define OPSIZ   (sizeof(op_t))
#define BYTE_COPY_FWD(dst_bp, src_bp, nbytes)                     \
  do                                          \
    {                                         \
      int __nbytes = (nbytes);                        \
      while (__nbytes > 0)                            \
    {                                     \
      byte __x = ((byte *) src_bp)[0];                    \
      src_bp += 1;                                \
      __nbytes -= 1;                              \
      ((byte *) dst_bp)[0] = __x;                         \
      dst_bp += 1;                                \
    }                                     \
    } while (0)
extern void _wordcopy_fwd_aligned (long int, long int, int);
extern void _wordcopy_fwd_dest_aligned (long int, long int, int);
#define WORD_COPY_FWD(dst_bp, src_bp, nbytes_left, nbytes)            \
  do                                          \
    {                                         \
      if (src_bp % OPSIZ == 0)                            \
    _wordcopy_fwd_aligned (dst_bp, src_bp, (nbytes) / OPSIZ);         \
      else                                    \
    _wordcopy_fwd_dest_aligned (dst_bp, src_bp, (nbytes) / OPSIZ);        \
      src_bp += (nbytes) & -OPSIZ;                        \
      dst_bp += (nbytes) & -OPSIZ;                        \
      (nbytes_left) = (nbytes) % OPSIZ;                       \
    } while (0)
typedef unsigned char byte;

#ifndef size_t
typedef __kernel_size_t size_t;
#endif

/*
#define LIST_TYPE(STRUCT) struct list_head ##_## STRUCT
#define LISTP_TYPE(STRUCT) struct listp ##_## STRUCT

#define listp_add_tail(NEW, HEAD, FIELD) do {               \
        if ((HEAD)->first == NULL) {                        \
            (HEAD)->first = (NEW);                          \
            (NEW)->FIELD.next = (NEW);                      \
            (NEW)->FIELD.prev = (NEW);                      \
        } else                                              \
            list_add_tail(NEW, (HEAD)->first, FIELD);       \
    } while (0)
*/
#define PRINT_ENCLAVE_STAT  (0)
/*
#define DEFINE_LIST(STRUCT)                     \
    struct STRUCT;                              \
    LIST_TYPE(STRUCT) {                         \
        struct STRUCT *next, *prev;             \
    }

#define DEFINE_LISTP(STRUCT)                    \
    LISTP_TYPE(STRUCT) {                        \
        struct STRUCT * first;                  \
    }

#define listp_empty(HEAD) ((HEAD)->first == NULL)
#define listp_first_entry(LISTP, TYPE, FIELD) ((LISTP)->first)
#define listp_del(NODE, HEAD, FIELD) do {                               \
        if ((HEAD)->first == (NODE)) {                                  \
            if ((NODE)->FIELD.next == NODE) {                           \
                (HEAD)->first = NULL;                                   \
            } else {                                                    \
                (HEAD)->first = (NODE)->FIELD.next;                     \
            }                                                           \
        }                                                               \
        LIST_ASSERT((NODE)->FIELD.prev->FIELD.next == (NODE));          \
        LIST_ASSERT((NODE)->FIELD.next->FIELD.prev == (NODE));          \
        (NODE)->FIELD.prev->FIELD.next = (NODE)->FIELD.next;            \
        (NODE)->FIELD.next->FIELD.prev = (NODE)->FIELD.prev;            \
    } while(0)

#define listp_add(NEW, HEAD, FIELD) do {                    \
        if ((HEAD)->first == NULL) {                        \
            (HEAD)->first = (NEW);                          \
            (NEW)->FIELD.next = (NEW);                      \
            (NEW)->FIELD.prev = (NEW);                      \
        } else {                                            \
            __list_add(NEW, (HEAD)->first, (HEAD)->first->FIELD.prev, FIELD); \
            (HEAD)->first = (NEW);                          \
        }                                                   \
    } while (0)

#define listp_prev_entry(NODE, LISTP, FIELD)                            \
        ((NODE) == (LISTP)->first ? NULL : (NODE)->FIELD.prev)
*/
#ifndef SLAB_LEVEL
#define SLAB_LEVEL      8
#endif

/*#define INIT_LIST_HEAD(OBJECT, FIELD) do {      \
        (OBJECT)->FIELD.next = NULL;            \
        (OBJECT)->FIELD.prev = NULL;            \
    } while (0)

#define __list_add(NEW, NEXT, PREV, FIELD) do {       \
        __typeof__(NEW) __tmp_next = (NEXT);          \
        __typeof__(NEW) __tmp_prev = (PREV);          \
        __tmp_prev->FIELD.next = (NEW);               \
        __tmp_next->FIELD.prev = (NEW);               \
        (NEW)->FIELD.next = __tmp_next;               \
        (NEW)->FIELD.prev = __tmp_prev;               \
    } while (0)
*/
#define __SUM_OBJ_SIZE(slab_size, size) \
    (((slab_size) + SLAB_HDR_SIZE) * (size))
#define __MIN_MEM_SIZE() (sizeof(SLAB_AREA_TYPE))
#define __MAX_MEM_SIZE(slab_size, size) \
    (__MIN_MEM_SIZE() + __SUM_OBJ_SIZE((slab_size), (size)))

#ifdef SLAB_DEBUG
struct slab_debug {
    struct {
        const char * file;
        int          line;
    } alloc, free;
};

# define SLAB_DEBUG_SIZE    sizeof(struct slab_debug)
#else
# define SLAB_DEBUG_SIZE    0
#endif

#ifdef SLAB_CANARY
# define SLAB_CANARY_STRING 0xDEADBEEF
# define SLAB_CANARY_SIZE   sizeof(unsigned long)
#else
# define SLAB_CANARY_SIZE   0
#endif

#define ROUND_UP(_x, _y) ((((_x) + (_y) - 1) / (_y)) * (_y))

#define SLAB_HDR_SIZE \
    ROUND_UP((sizeof(SLAB_OBJ_TYPE) - sizeof(LIST_TYPE(slab_obj)) +     \
                SLAB_DEBUG_SIZE + SLAB_CANARY_SIZE),                      \
            MIN_MALLOC_ALIGNMENT)

#ifndef SLAB_LEVEL_SIZES
# define SLAB_LEVEL_SIZES  16, 32, 64,           \
    128  - SLAB_HDR_SIZE, \
    256  - SLAB_HDR_SIZE, \
    512  - SLAB_HDR_SIZE, \
    1024 - SLAB_HDR_SIZE, \
    2048 - SLAB_HDR_SIZE
# define SLAB_LEVELS_SUM  (4080 - SLAB_HDR_SIZE * 5)
//#else
//# ifndef SLAB_LEVELS_SUM
//# error "SALB_LEVELS_SUM not defined"
//# endif
#endif

#define AREA_PADDING            12
#define OBJ_PADDING             15
#define LARGE_OBJ_PADDING       8
#define MIN_MALLOC_ALIGNMENT    16
#define STATIC_SLAB              1

#ifdef IN_ENCLAVE
#define PAL_ALLOC_INTERNAL    0x8000
#endif

#define OBJ_LEVEL(obj) ((obj)->level)
#define OBJ_RAW(obj) (&(obj)->raw)

#ifndef system_lock
#define system_lock() spinlock_lock(&slab_lock)
#endif
#ifndef system_unlock
#define system_unlock() spinlock_unlock(&slab_lock)
#endif

DEFINE_LIST(slab_area);

typedef struct __attribute__((packed)) slab_area {
    LIST_TYPE(slab_area) __list;
    unsigned int size;
    unsigned char pad[AREA_PADDING];
    //TODO
    unsigned char raw[32];
} SLAB_AREA_TYPE, * SLAB_AREA;

DEFINE_LIST(slab_obj);

typedef struct __attribute__((packed)) slab_obj {
    unsigned char level;
    unsigned char padding[OBJ_PADDING];
//    union {
        LIST_TYPE(slab_obj) __list;
        unsigned char *raw;
//    };
} SLAB_OBJ_TYPE, * SLAB_OBJ;

DEFINE_LISTP(slab_obj);
DEFINE_LISTP(slab_area);
typedef struct slab_mgr {
    LISTP_TYPE(slab_area) area_list[SLAB_LEVEL];
    LISTP_TYPE(slab_obj) free_list[SLAB_LEVEL];
    size_t size[SLAB_LEVEL];
    void * addr[SLAB_LEVEL], * addr_top[SLAB_LEVEL];
    SLAB_AREA active_area[SLAB_LEVEL];
} SLAB_MGR_TYPE, * SLAB_MGR;


#define __INIT_SUM_OBJ_SIZE(size) \
    ((SLAB_LEVELS_SUM + SLAB_HDR_SIZE * SLAB_LEVEL) * (size))
#define __INIT_MIN_MEM_SIZE() \
    (sizeof(SLAB_MGR_TYPE) + sizeof(SLAB_AREA_TYPE) * SLAB_LEVEL)
#define __INIT_MAX_MEM_SIZE(size) \
    (__INIT_MIN_MEM_SIZE() + __INIT_SUM_OBJ_SIZE((size)))

#endif
