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

#ifndef API_H
#define API_H


#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

// #include "native_client/src/trusted/service_runtime/include/sys/errno.h"
#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/stdlib/list.h"

/* WARNING: this declaration may conflict with some header files */
#ifndef ssize_t
typedef ptrdiff_t ssize_t;
#endif

/* Macros */

#ifndef likely
# define likely(x)  __builtin_expect((!!(x)),1)
#endif
#ifndef unlikely
# define unlikely(x)    __builtin_expect((!!(x)),0)
#endif

#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define IS_POWER_OF_2(x) \
    ({ assert((x) != 0); \
       (((x) & ((x) - 1)) == 0); })


#define IS_ALIGNED(val, alignment) ((val) % (alignment) == 0)
#define ALIGN_DOWN(val, alignment) ((val) - (val) % (alignment))
#define ALIGN_UP(val, alignment)   ALIGN_DOWN((val) + (alignment) - 1, alignment)
#define IS_ALIGNED_PTR(val, alignment) IS_ALIGNED((uintptr_t)(val), alignment)
#define ALIGN_DOWN_PTR(ptr, alignment) ((__typeof__(ptr))(ALIGN_DOWN((uintptr_t)(ptr), alignment)))
#define ALIGN_UP_PTR(ptr, alignment)   ((__typeof__(ptr))(ALIGN_UP((uintptr_t)(ptr), alignment)))

/* Useful only when the alignment is a power of two, but when that's not known compile-time. */
#define IS_ALIGNED_POW2(val, alignment) (((val) & ((alignment) - 1)) == 0)
#define ALIGN_DOWN_POW2(val, alignment) \
    ((val) - ((val) & ((alignment) - 1))) // `~` doesn't work if `alignment` is of a smaller type
                                          // than `val` and unsigned.
#define ALIGN_UP_POW2(val, alignment)       ALIGN_DOWN_POW2((val) + (alignment) - 1, alignment)
#define IS_ALIGNED_PTR_POW2(val, alignment) IS_ALIGNED_POW2((uintptr_t)(val), alignment)
#define ALIGN_DOWN_PTR_POW2(ptr, alignment) ((__typeof__(ptr))(ALIGN_DOWN_POW2((uintptr_t)(ptr), \
                                                                               alignment)))
#define ALIGN_UP_PTR_POW2(ptr, alignment)   ((__typeof__(ptr))(ALIGN_UP_POW2((uintptr_t)(ptr), \
                                                                             alignment)))

#define SAME_TYPE(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))
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

#define __alloca __builtin_alloca

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x

#define static_strlen(str) (sizeof(str) - 1)

#define COMPILER_BARRIER() ({ __asm__ __volatile__ ("" ::: "memory"); })

#define READ_ONCE(x) ({                     \
    __typeof__(x) _y;                       \
    COMPILER_BARRIER();                     \
    __builtin_memcpy(&_y, &(x), sizeof(x)); \
    COMPILER_BARRIER();                     \
    _y; })

#define WRITE_ONCE(x, y) ({                 \
    __typeof__(x) _y = (__typeof__(x))(y);  \
    COMPILER_BARRIER();                     \
    __builtin_memcpy(&(x), &_y, sizeof(x)); \
    COMPILER_BARRIER();                     \
    _y; })


/* Libc functions */

/* Libc String functions string.h/stdlib.h */
//size_t strnlen (const char *str, size_t maxlen);
//size_t strlen (const char *str);
//int strncmp (const char *s1, const char *s2, int n);
int strcmp(const char *s1, const char *s2);
char * strncpy(char *dst, const char *src, size_t n);
char * strpbrk(const char *s1, const char *s2);
char * strdup (const char *s);

#if NACL_SGX == 1//NACL_PAVE
extern int errno;
#else

#endif
// 
// int errno;
long strtol (const char *s, char **endptr, int base);
int atoi (const char *nptr);
long int atol (const char *nptr);

char * strchr (const char *s, int c_in);
unsigned long strtoul(const char *nptr, char **endptr, register int base);

void * memcpy (void *dstpp, const void *srcpp, size_t len);
void * memmove (void *dstpp, const void *srcpp, size_t len);
void * memset (void *dstpp, int c, size_t len);
int memcmp (const void *s1, const void *s2, size_t len);

/* Libc memory allocation functions. stdlib.h. */
void *malloc(size_t size);
void *realloc (void *ptr, size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);

void qsort(void *aa, size_t n, size_t es, int (*cmp)(const void *, const void *));
void * bsearch(const void *key, const void *base0, size_t nmemb, size_t size,
    int (*compar)(const void *, const void *));

/* Some useful macro */
/* force failure if str is not a static string */
#define force_static(str)   ("" str "")

/* check if the var is exactly the same as the static string */
#define strcmp_static(var, str) \
    (!memcmp((var), force_static(str), static_strlen(force_static(str)) + 1))

/* check if the var starts with the static string */
#define strpartcmp_static(var, str) \
    (!memcmp((var), force_static(str), static_strlen(force_static(str))))

/* copy static string and return the address of the null end (null if the dest
 * is not large enough).*/
#define strcpy_static(var, str, max) \
    (static_strlen(force_static(str)) + 1 > max ? NULL : \
     memcpy((var), force_static(str), static_strlen(force_static(str)) + 1) + \
     static_strlen(force_static(str)))

/* Libc printf functions. stdio.h/stdarg.h. */
void fprintfmt (int (*_fputch)(void *, int, void *), void * f, void * putdat,
                const char * fmt, ...);

void vfprintfmt (int (*_fputch)(void *, int, void *), void * f, void * putdat,
                 const char * fmt, va_list *ap);

//int snprintf (char * buf, int n, const char * fmt, ...);

/* Miscelleneous */

int inet_pton4 (const char *src, int len, void *dst);
int inet_pton6 (const char *src, int len, void *dst);

uint32_t __htonl (uint32_t x);
uint32_t __ntohl (uint32_t x);
uint16_t __htons (uint16_t x);
uint16_t __ntohs (uint16_t x);

extern const char * const * sys_errlist_internal;

/* Graphene functions */

int get_norm_path (const char * path, char * buf, int offset, int size);

int get_base_name (const char * path, char * buf, int size);

/* Loading configs / manifests */


struct config;
DEFINE_LISTP(config);
struct config_store {
    LISTP_TYPE(config) root;
    LISTP_TYPE(config) entries;
    void *           raw_data;
    int              raw_size;
    void *           (*malloc) (size_t);
    void             (*free) (void *);
};

int read_config (struct config_store * store, int (*filter) (const char *, int),
                 const char ** errstring);
int free_config (struct config_store * store);
int copy_config (struct config_store * store, struct config_store * new_store);
int write_config (void * file, int (*write) (void *, void *, int),
                  struct config_store * store);
ssize_t get_config (struct config_store * cfg, const char * key,
                    char * val_buf, size_t buf_size);
int get_config_entries (struct config_store * cfg, const char * key,
                        char * key_buf, size_t key_bufsize);
ssize_t get_config_entries_size (struct config_store * cfg, const char * key);
int set_config (struct config_store * cfg, const char * key, const char * val);

int vsnprintf (char *string, size_t maxlen, const char *format, va_list args);

#define CONFIG_MAX      4096


#endif /* API_H */
