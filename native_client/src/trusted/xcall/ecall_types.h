#ifndef _ECALL_TYPES_H_
#define _ECALL_TYPES_H_


enum {
  ECALL_ENCLAVE_START = 0,
  ECALL_NR,
};

typedef struct {
	char *nap;
} ms_ecall_enclave_start_t;



#endif
