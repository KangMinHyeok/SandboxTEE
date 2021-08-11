#include <asm/errno.h>
#include <sys/socket.h>

#include "native_client/src/shared/platform/nacl_check.h"
#include "native_client/src/shared/platform/nacl_host_desc.h"
#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_sync.h"
#include "native_client/src/shared/platform/nacl_sync_checked.h"

#include "native_client/src/trusted/service_runtime/nacl_config.h"

#include "native_client/src/trusted/service_runtime/include/sys/errno.h"
#include "native_client/src/trusted/service_runtime/include/sys/fcntl.h"

#include "native_client/src/shared/platform/sgx/linux_types.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"


static int32_t NaClHostIfcCtor(struct NaClHostDesc  *d, int sock_desc){

  d->d = sock_desc;
  d->flags = NACL_ABI_O_RDWR;
  return 0;
}


int NaClHostIfcOpen(struct NaClHostDesc *d){
  // int fd;
  int rv = 1;

  NaClLog(3, "NaClHostIfcOpen(0x%08"NACL_PRIxPTR")\n", (uintptr_t) d);
  if (NULL == d) {
    NaClLog(LOG_FATAL, "NaClHostIfcOpen: 'this' is NULL\n");
  }

  // dummy descriptor
  rv = NaClHostIfcCtor(d, 0);
  return rv;
}

int NaClHostIfcBind(struct NaClHostDesc *d, struct sockaddr *addr){
  
  int rv = 1;
  unsigned int addrlen = sizeof(*addr);
 
  NaClLog(3, "NaClHostIfcBind(0x%08"NACL_PRIxPTR")\n", (uintptr_t) d);
  if (NULL == d) {
    NaClLog(LOG_FATAL, "NaClHostIfcBind: 'this' is NULL\n");
  }

  rv = ocall_sock_listen(AF_INET, SOCK_STREAM, 0, 
      addr, &addrlen);
  if (rv < 0) {
    NaClLog(4, "NaClHostIfcBindAndListen error %d\n", rv);
    return rv;
  }
      
  rv = NaClHostIfcCtor(d, rv);
  if (rv!=0)
    NaClLog(4, "NaClHostIfcBindAndListen error %d\n", rv);
 
  NaClLog(3, "NaClHostIfcBindAndListen returns %d\n", rv);
  return rv;
}

/*
int NaClHostIfcListen(struct NaClHostDesc *d, int backlog){

  UNREFERENCED_PARAMETER(d);
  UNREFERENCED_PARAMETER(backlog);

  NaClLog(LOG_FATAL, "this function shouldn't be reached\n");
  return 0;
}
*/

struct NaClHostDesc* NaClHostIfcAccept(struct NaClHostDesc *d){

  int rv = 1;
  struct NaClHostDesc *hd; 
 
  NaClLog(3, "NaClHostIfcAccept(0x%08"NACL_PRIxPTR")\n", (uintptr_t) d);
  if (NULL == d || d->d <= 0) {
    NaClLog(LOG_FATAL, "NaClHostIfcAccept: 'this' is NULL\n");
  }

  rv = ocall_sock_accept(d->d, NULL, NULL); // NULL, NULL

  if (rv <= 0)
    return NULL;

  
  hd = malloc(sizeof *hd);
  if (hd == NULL) {
    return NULL;
  }

  rv = NaClHostIfcCtor(hd, rv);

  if (rv < 0) { 
    // It never reaches here
    free(hd);
    return NULL;
  }

  NaClLog(3, "NaClHostIfcAccept %d\n", rv);
  return hd;
}

/* Internet address. */
struct in_addr {
  uint32_t       s_addr;     /* address in network byte order */
};

struct sockaddr_in {
  sa_family_t    sin_family; /* address family: AF_INET */
  uint16_t      sin_port;   /* port in network byte order */
  struct in_addr sin_addr;   /* internet address */
  char sin_zero[8];
};


int NaClHostIfcConnect(struct NaClHostDesc *d, struct sockaddr *addr){

  int rv = 1;
 
  NaClLog(3, "NaClHostIfcConnect(0x%08"NACL_PRIxPTR")\n", (uintptr_t) d);
  if (NULL == d) {
    NaClLog(LOG_FATAL, "NaClHostIfcConnect: 'this' is NULL\n");
  }

  NaClLog(3, "0. %d %d\n", ((struct sockaddr_in *)addr)->sin_addr.s_addr, ((struct sockaddr_in *)addr)->sin_port);

  rv = ocall_sock_connect(AF_INET, SOCK_STREAM, 0, addr, sizeof(struct sockaddr), NULL, NULL);
    
  rv = NaClHostIfcCtor(d, rv);
  if (rv!=0)
    NaClLog(4, "NaClHostIfcConnect error %d\n", rv);
 
  NaClLog(3, "NaClHostIfcConnect returns %d\n", rv);

  return rv;
}

int NaClHostIfcRead(struct NaClHostDesc *d, void *buf, unsigned int count){

  int rv = 1;
 
  NaClLog(3, "NaClHostIfcRead(0x%08"NACL_PRIxPTR")\n", (uintptr_t) d);
  if (NULL == d || d->d <= 0) {
    NaClLog(LOG_FATAL, "NaClHostIfcRead: 'this' is NULL\n");
  }

  rv = ocall_read(d->d, buf, count);

  return rv;
}

int NaClHostIfcWrite(struct NaClHostDesc *d, void *buf, unsigned int count){

  int rv = 1;
 
  NaClLog(3, "NaClHostIfcWrite(0x%08"NACL_PRIxPTR")\n", (uintptr_t) d);
  if (NULL == d || d->d <= 0) {
    NaClLog(LOG_FATAL, "NaClHostIfcWrite: 'this' is NULL\n");
  }

  rv = ocall_write(d->d, buf, count);

  return rv;
}

int NaClHostIfcClose(struct NaClHostDesc *d){

  NaClLog(3, "NaClHostIfcClose(0x%08"NACL_PRIxPTR")\n", (uintptr_t) d);
  if (NULL == d || d->d <= 0) {
    NaClLog(LOG_FATAL, "NaClHostIfcClose: 'this' is NULL\n");
  }

  ocall_close(d->d);
  return 0;
}

