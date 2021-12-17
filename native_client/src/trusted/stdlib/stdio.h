#include "native_client/src/trusted/stdlib/fileio.h"
#include "native_client/src/include/build_config.h"
#include <stdarg.h>

#if NACL_SGX == 1
/* Standard streams.  */
extern FILE *stdin;                /* Standard input stream.  */
extern FILE *stdout;                /* Standard output stream.  */
extern FILE *stderr;                /* Standard error output stream.  */
/* C89/C99 say they're macros.  Make them happy.  */
#define stdin stdin
#define stdout stdout
#define stderr stderr

void fprintfmt (int (*_fputch)(void *, int, void *), void * f, void * putdat,
                const char * fmt, ...);

void vfprintfmt (int (*_fputch)(void *, int, void *), void * f, void * putdat,
                 const char * fmt, va_list *ap);


int open(const char *__file, int __oflag, ...);

//int setvbuf(FILE *fp, int mode, size_t size);
int setvbuf (FILE *__restrict __stream, char *__restrict __buf, int __modes, size_t __n);
#endif 
#define EOF (-1)

