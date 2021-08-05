#ifndef __ENCALVE_OCALLS__
#define __ENCLAVE_OCALLS__


#include <time.h>


// Time
int ocall_gettimeofday (struct timeval *tv);
int ocall_clock_gettime (int clk_id, struct timespec *tp);


int ocall_debugp (int val);
int ocall_print_string(char * buf, int idx);




#endif
