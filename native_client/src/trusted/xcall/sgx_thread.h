#ifndef __SGX_THREAD__
#define __SGX_THREAD__




void create_tcs_mapper (void * tcs_base, unsigned int thread_num);


void map_tcs (unsigned int tid);



#endif
