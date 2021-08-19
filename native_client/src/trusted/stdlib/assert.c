#include "native_client/src/include/build_config.h"
// #include "native_client/src/trusted/stdlib/assert.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"

#include <stdio.h>


/* This does not return */
void __abort(void) {
    ocall_exit(1);
}

void warn (const char *format, ...)
{

}

void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function)
{
    
    printf ("%s%s:%u: %s%sAssertion `%s' failed.\n", assertion, file, line, function);
    __abort();
}
