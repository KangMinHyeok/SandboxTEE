#ifndef MEMHEADER_H
#define MEMHEADER_H

#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/trusted/stdlib/nacl_error.h"

#if NACL_SGX == 0
#error "it should be included only with NACL_SGX == 1"
#endif

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

void init_slab_mgr (int alignment);

void * memcpy (void *dstpp, const void *srcpp, size_t len);
void * memmove (void *dstpp, const void *srcpp, size_t len);
void * memset (void *dstpp, int c, size_t len);
int memcmp (const void *s1, const void *s2, size_t len);
void *memalign(size_t align, size_t len);

void *malloc(size_t size);
void *realloc (void *ptr, size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);

static inline void cpu_pause(void) {
    __asm__ volatile("pause");
}

#endif
