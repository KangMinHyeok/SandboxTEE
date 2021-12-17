/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

#ifndef _GATOMIC_H_
#define _GATOMIC_H_

/* Copyright (C) 2014 Stony Brook University
 * Copyright (C) 2017 Fortanix Inc, and University of North Carolina 
 * at Chapel Hill.
 *
 * This file defines atomic operations (And barriers) for use in 
 * Graphene.
 * 
 * The atomic operation assembly code is taken from musl libc, which 
 * is subject to the MIT license.
 * 
 * At this point, we primarily focus on x86_64; there are some vestigial
 * 32-bit definitions here, but a more portable version would need to 
 * move and reimplement portions of this for 32-bit x86 (or other architectures).
 */

/*
/----------------------------------------------------------------------
Copyright (C) 2005-2014 Rich Felker, et al.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    ----------------------------------------------------------------------
*/

/* Optimization barrier */
#define barrier()    __asm__ __volatile__("": : :"memory")
# define cpu_relax() __asm__ __volatile__("rep; nop" ::: "memory");

#ifdef __i386__
# define rmb()      __asm__ __volatile__("lock; addl $0,0(%%esp)" ::: "memory")

}
#ifdef __GNUC__
__attribute__((aligned(sizeof(uint32_t))))
#endif
;
#endif


#define __x64_64__
/* The return types below effectively assume we are dealing with a 64-bit
 * signed value. 
 */
#ifdef __x86_64__
/*
 * Some non-Intel clones support out of order store. wmb() ceases to be a
 * nop for these.
 */
# define mb()    __asm__ __volatile__ ("mfence" ::: "memory")
# define rmb()   __asm__ __volatile__ ("lfence" ::: "memory")
# define wmb()   __asm__ __volatile__ ("sfence" ::: "memory")
/*
gatomic_int {
    volatile int64_t counter;
} */
typedef volatile int32_t gatomic_int
#ifdef __GNUC__
__attribute__((aligned(sizeof(uint64_t))))
#endif
;
#endif

#define LOCK_PREFIX     "\n\tlock; "

#define ATOMIC_INIT(i)      { (i) }

/* Read the value currently stored in the gatomic_int */
static inline int32_t iatomic_read (const gatomic_int * v)
{
    //  Effectively:
    //      return v;
    int64_t i;
    /* Use inline assembly to ensure this is one instruction */
    __asm__ __volatile__("mov %1, %0"
                         : "=r"(i) :
                           "m"(v) : "memory");
    return i;
}

/* Does a blind write to the atomic variable */
static inline void iatomic_set (gatomic_int * v, int32_t i)
{
    //  Effectively:
    //      v = i;
    /* Use inline assembly to ensure this is one instruction */
    __asm__ __volatile__("mov %1, %0"
                         : "=r"(*v) 
                         : "r"(i) );
}

/* Helper function that atomically adds a value to an gatomic_int,
 * and returns the _new_ value. */
static inline int32_t _iatomic_add (int32_t i, gatomic_int * v)
{
    int32_t increment = i;
    __asm__ __volatile__(
        "lock ; xadd %0, %1"
        : "=r"(i), "=m"(v) : "0"(i) : "memory" );
    return i + increment;
}

/* Atomically adds i to v.  Does not return a value. */
static inline void iatomic_add (int32_t i, gatomic_int * v)
{
    _iatomic_add(i, v);
}

/* Atomically substracts i from v.  Does not return a value. */
static inline void iatomic_sub (int32_t i, gatomic_int * v)
{
    _iatomic_add(-i, v);
}

/* Atomically adds 1 to v.  Does not return a value. */
static inline void iatomic_inc (gatomic_int * v)
{
    __asm__ __volatile__(
        "lock ; incl %0"
        : "=m"(v) : "m"(v) : "memory" );
}

/* Atomically substracts 1 from v.  Does not return a value. */
static inline void iatomic_dec (gatomic_int * v)
{
    __asm__ __volatile__(
        "lock ; decl %0"
        : "=m"(v) : "m"(v) : "memory" );
}

/* Atomically substracts 1 from v.  Returns 1 if this causes the 
   value to reach 0; returns 0 otherwise. */
static inline int32_t iatomic_dec_and_test (gatomic_int * v)
{
    int32_t i = _iatomic_add(-1, v);
    return i == 0;
}
 
/* Helper function to atomically compare-and-swap the value pointed to by p.
 * t is the old value, s is the new value.  Returns 
 * the value originally in p. */
static inline int32_t icmpxchg(volatile int32_t *p, int32_t t, int32_t s)
{
    __asm__ __volatile__ (
        "lock ; cmpxchg %3, %1"
        : "=a"(t), "=m"(*p) : "a"(t), "r"(s) : "memory" );
    return t;
}

/* Helper function to atomically compare-and-swap the value in v.
 * If v == old, it sets v = new.
 * Returns the value originally in v. */
static inline int32_t iatomic_cmpxchg (gatomic_int * v, int32_t old, int32_t
        newv)
{
    return icmpxchg(v, old, newv);
}

/* Helper function to atomically swap the value pointed to by p.
 * t is the old value, s is the new value.  Returns 
 * the value originally in p. */
static inline int32_t ixchg(volatile int32_t *p, int32_t s)
{
    uint32_t t = 0;
    __asm__ __volatile__ (
        "lock ; xchg %2, %1"
        : "=a"(t), "=m"(*p) : "r"(s) : "memory" );
    return t;
}

/* Helper function to atomically swap the value in v.
 * If v == old, it sets v = new.
 * Returns the value originally in v. */
static inline int32_t iatomic_xchg (gatomic_int * v, int32_t newv)
{
    return ixchg(v, newv);
}


#endif /* _gatomic_int_H_ */
