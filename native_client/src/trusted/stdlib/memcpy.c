/* Copy memory to memory until the specified number of bytes
         has been copied.  Overlap is NOT handled correctly.
         Copyright (C) 1991, 1997, 2003 Free Software Foundation, Inc.
         This file is part of the GNU C Library.
         Contributed by Torbjorn Granlund (tege@sics.se).

         The GNU C Library is free software; you can redistribute it and/or
         modify it under the terms of the GNU Lesser General Public
         License as published by the Free Software Foundation; either
         version 2.1 of the License, or (at your option) any later version.

         The GNU C Library is distributed in the hope that it will be useful,
         but WITHOUT ANY WARRANTY; without even the implied warranty of
         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
         Lesser General Public License for more details.

         You should have received a copy of the GNU Lesser General Public
         License along with the GNU C Library; if not, write to the Free
         Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
         02111-1307 USA.  */

#include "api.h"
//#include <sysdeps/generic/memcopy.h>
#include "memcopy.h"
int printf  (const char  *fmt, ...);

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
