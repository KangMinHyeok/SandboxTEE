/* Copyright (C) 1991,1993-1997,1999,2000,2003,2006
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Based on strlen implementation by Torbjorn Granlund (tege@sics.se),
   with help from Dan Sahlin (dan@sics.se) and
   bug fix and commentary by Jim Blandy (jimb@ai.mit.edu);
   adaptation to strchr suggested by Dick Karpinski (dick@cca.ucsf.edu),
   and implemented by Roland McGrath (roland@ai.mit.edu).

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

//#include <api.h>
// #include <ctype.h>
#include "native_client/src/trusted/stdlib/string.h"
#include "native_client/src/trusted/stdlib/memory.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1
bool isspace(int c) {
  return (c == ' ' || c == '\t'); // || whatever other char you consider space
}

bool isdigit(int c) {
  return (c <= '9' && c >= '0');
}

bool isalpha(int c) {
  return ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A'));
}

bool isupper(int c) {
  return (c <= 'Z' && c >= 'A');
}


#define ULONG_MAX 0xFFFFFFFFUL


/* Find the first occurrence of C in S.  */
char * strchr (const char *s, int c_in)
{
  const unsigned char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, magic_bits, charmask;
  unsigned char c;

  c = (unsigned char) c_in;

  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = (const unsigned char *) s;
      ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0;
      ++char_ptr)
    if (*char_ptr == c)
      return (void *) char_ptr;
    else if (*char_ptr == '\0')
      return NULL;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */

  longword_ptr = (unsigned long int *) char_ptr;

  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:

bits:  01111110 11111110 11111110 11111111
bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

The 1-bits make sure that carries propagate to the next 0-bit.
The 0-bits provide holes for carries to fall into.  */
  switch (sizeof (longword)) {
    case 4: magic_bits = 0x7efefeffL; break;
    case 8: magic_bits = ((0x7efefefeL << 16) << 16) | 0xfefefeffL; break;
    default:
            return NULL;
  }

  /* Set up a longword, each of whose bytes is C.  */
  charmask = c | (c << 8);
  charmask |= charmask << 16;
  if (sizeof (longword) > 4)
    /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
    charmask |= (charmask << 16) << 16;
  if (sizeof (longword) > 8)
    return NULL;

  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;) {
    /* We tentatively exit the loop if adding MAGIC_BITS to
       LONGWORD fails to change any of the hole bits of LONGWORD.

       1) Is this safe?  Will it catch all the zero bytes?
       Suppose there is a byte with all zeros.  Any carry bits
       propagating from its left will fall into the hole at its
       least significant bit and stop.  Since there will be no
       carry from its most significant bit, the LSB of the
       byte to the left will be unchanged, and the zero will be
       detected.

       2) Is this worthwhile?  Will it ignore everything except
       zero bytes?  Suppose every byte of LONGWORD has a bit set
       somewhere.  There will be a carry into bit 8.  If bit 8
       is set, this will carry into bit 16.  If bit 8 is clear,
       one of bits 9-15 must be set, so there will be a carry
       into bit 16.  Similarly, there will be a carry into bit
       24.  If one of bits 24-30 is set, there will be a carry
       into bit 31, so all of the hole bits will be changed.

       The one misfire occurs when bits 24-30 are clear and bit
       31 is set; in this case, the hole at bit 31 is not
       changed.  If we had access to the processor carry flag,
       we could close this loophole by putting the fourth hole
       at bit 32!

       So it ignores everything except 128's, when they're aligned
       properly.

       3) But wait!  Aren't we looking for C as well as zero?
       Good point.  So what we do is XOR LONGWORD with a longword,
       each of whose bytes is C.  This turns each byte that is C
       into a zero.  */

    longword = *longword_ptr++;

    /* Add MAGIC_BITS to LONGWORD.  */
    if ((((longword + magic_bits)
            /* Set those bits that were unchanged by the addition.  */
            ^ ~longword)

          /* Look at only the hole bits.  If any of the hole bits
             are unchanged, most likely one of the bytes was a
             zero.  */
          & ~magic_bits) != 0 ||

        /* That caught zeroes.  Now test for C.  */
        ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask))
         & ~magic_bits) != 0) {
      /* Which of the bytes was C or zero?
         If none of them were, it was a misfire; continue the search.  */

      const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);

      if (*cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (*++cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (*++cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (*++cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (sizeof (longword) > 4) {
        if (*++cp == c)
          return (char *) cp;
        else if (*cp == '\0')
          return NULL;
        if (*++cp == c)
          return (char *) cp;
        else if (*cp == '\0')
          return NULL;
        if (*++cp == c)
          return (char *) cp;
        else if (*cp == '\0')
          return NULL;
        if (*++cp == c)
          return (char *) cp;
        else if (*cp == '\0')
          return NULL;
      }
    }
  }

  return NULL;
}
  char *
strpbrk(const char *s1, const char *s2)
{
  const char *scanp;
  int c, sc;

  while ((c = *s1++) != 0) {
    for (scanp = s2; (sc = *scanp++) != 0;)
      if (sc == c)
        return ((char *)(s1 - 1));
  }
  return (NULL);
}

// TODO(mhkang): delete strncpy, it is moved to strcpy.c
/*
  char *
strncpy(char *dst, const char *src, size_t n)
{
  if (n != 0) {
    char *d = dst;
    const char *s = src;
    do { 
      if ((*d++ = *s++) == 0) {
        // NUL pad the remaining n-1 bytes
        while (--n != 0)
          *d++ = 0;
        break;
      }
    } while (--n != 0);
  }
  return (dst);
}
*/
  int
strcmp(const char *s1, const char *s2)
{
  while (*s1 == *s2++)
    if (*s1++ == 0)
      return (0);
  return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}
  unsigned long
strtoul(const char *nptr, char **endptr, register int base)
{
  register const char *s = nptr;
  register unsigned long acc;
  register int c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   *   * See strtol for comments as to the logic used.
   *     */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;
  cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
  cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = ULONG_MAX;
    // errno = ERANGE;
  } else if (neg)
    acc = -acc;
  if (endptr != 0)
    *endptr = (char *)(any ? s - 1 : nptr);
  return (acc);
}

char * sgx_strdup (const char *s)
{
  size_t len = strlen(s) + 1;
  char *new_s = (char *) malloc(len);

  if (new_s)
    memcpy(new_s, s, len);

  return new_s;
}

#endif
