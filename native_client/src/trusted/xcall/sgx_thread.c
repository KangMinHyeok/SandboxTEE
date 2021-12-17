#include "native_client/src/trusted/service_runtime/sgx/sgx_arch.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

__thread sgx_arch_tcs_t * current_tcs;

struct thread_map {
    unsigned int     tid;
    sgx_arch_tcs_t * tcs;
};

static sgx_arch_tcs_t * enclave_tcs;
static int enclave_thread_num;
static struct thread_map * enclave_thread_map;
//int num_cores = 6;
//int core_index = 1;

void create_tcs_mapper (void * tcs_base, unsigned int thread_num)
{
    enclave_tcs = tcs_base;
    enclave_thread_map = malloc(sizeof(struct thread_map) * thread_num);
    enclave_thread_num = thread_num;

    for (unsigned int i = 0 ; i < thread_num ; i++) {
        enclave_thread_map[i].tid = 0;
        enclave_thread_map[i].tcs = &enclave_tcs[i];
    }

    // num_cores = sysconf(_SC_NPROCESSORS_ONLN);
}

void map_tcs (unsigned int tid)
{
    for (int i = 0 ; i < enclave_thread_num ; i++)
        if (!enclave_thread_map[i].tid) {
            enclave_thread_map[i].tid = tid;
            current_tcs = enclave_thread_map[i].tcs;
            // ((struct enclave_dbginfo *) DBGINFO_ADDR)->thread_tids[i] = tid;
            break;
        }

}

void unmap_tcs (void)
{
    int index = current_tcs - enclave_tcs;
    struct thread_map * map = &enclave_thread_map[index];
    if (index >= enclave_thread_num)
        return;
    current_tcs = NULL;
    // ((struct enclave_dbginfo *) DBGINFO_ADDR)->thread_tids[index] = 0;
    map->tid = 0;
    map->tcs = NULL;
}
/*
static void * thread_start (void * arg)
{
    int tid = gettid();
    map_tcs(tid);
    // current_enclave = arg;
		
		(void) arg;

    if (!current_tcs) {
        printf("Cannot attach to any TCS!\n");
        return NULL;
    }

    // ecall_thread_start();
    unmap_tcs();
    return NULL;
}
*/
int clone_thread (void)
{
    // pthread_t thread;
    return 0; // pthread_create(&thread, NULL, thread_start, NULL); // current_enclave);
}
