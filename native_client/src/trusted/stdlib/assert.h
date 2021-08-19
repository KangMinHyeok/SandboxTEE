/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

/*
 * assert.h
 *
 * Define a common interface for assertions that builds for both the PAL
 * and libOS.
 *
 */

#ifndef ASSERT_H
#define ASSERT_H
/*
#define COMPILE_TIME_ASSERT(pred) switch(0){case 0:case pred:;}
*/
/* All environments should implement warn, which prints a non-optional debug
 * message. All environments should also implement __abort, which
 * terminates the process.
 */

void warn (const char *format, ...);
void __abort(void);
//# ifdef assert
//# undef assert
/*
# define assert(test)                                                   \
    ({                                                                  \
        long _val = (long) (test);                                      \
        (!(_val))                                                       \
            ? ({                                                        \
                    warn("assert failed " __FILE__ ":%d " #test " (value:%x)\n", \
                         __LINE__, _val);                               \
                    __abort(); })                                       \
            : (void) 0;                                                 \
    })
//##else
*/

/*void
__assert_fail_base (const char *fmt, const char *assertion, const char *file,
		    unsigned int line, const char *function)
{
  char *str;

  int total;
  if (__asprintf (&str, fmt,
		  __progname, __progname[0] ? ": " : "",
		  file, line,
		  function ? function : "", function ? ": " : "",
		  assertion, &total) >= 0)
    {
      // Print the message.  /
      (void) __fxprintf (NULL, "%s", str);
      (void) fflush (stderr);

      total = (total + 1 + GLRO(dl_pagesize) - 1) & ~(GLRO(dl_pagesize) - 1);
      struct abort_msg_s *buf = __mmap (NULL, total, PROT_READ | PROT_WRITE,
					MAP_ANON | MAP_PRIVATE, -1, 0);
      if (__glibc_likely (buf != MAP_FAILED))
	{
	  buf->size = total;
	  strcpy (buf->msg, str);

	  // We have to free the old buffer since the application might
 	  //   catch the SIGABRT signal.
	  struct abort_msg_s *old = atomic_exchange_acq (&__abort_msg, buf);

	  if (old != NULL)
	    __munmap (old, old->size);
	}

      free (str);
    }
  else
    {
      // At least print a minimal message.  /
      static const char errstr[] = "Unexpected error.\n";
      __libc_write (STDERR_FILENO, errstr, sizeof (errstr) - 1);
    }
  abort ();
  // TODO: impl
}


#undef __assert_fail
void
__assert_fail (const char *assertion, const char *file, unsigned int line,
	       const char *function)
{
  __assert_fail_base ("%s%s%s:%u: %s%sAssertion `%s' failed.\n%n", assertion, file, line, function);
	__abort();
}

  */ //TODO crhamm
//#endif 

#endif
