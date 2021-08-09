#include "native_client/src/trusted/stdlib/fileio.h"
#include "native_client/src/include/build_config.h"

#if NACL_SGX == 1
/* Standard streams.  */
extern FILE *stdin;                /* Standard input stream.  */
extern FILE *stdout;                /* Standard output stream.  */
extern FILE *stderr;                /* Standard error output stream.  */
/* C89/C99 say they're macros.  Make them happy.  */
#define stdin stdin
#define stdout stdout
#define stderr stderr
#endif 

#define EOF (-1)

