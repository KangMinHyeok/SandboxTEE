
// #include "native_client/src/untrusted/ifc/custom.h"


#include <errno.h>

#include "native_client/src/untrusted/nacl/syscall_bindings_trampoline.h"
#include "native_client/src/untrusted/ifc/ifc.h"


int ifc_socket(void){
  int sockfd;
  NACL_SYSCALL(ifc_socket)(&sockfd);
  return sockfd;
}

int ifc_bind(int s, struct sockaddr *addr, unsigned int addrlen){
  return NACL_SYSCALL(ifc_bind)(s, (uint32_t) addr, addrlen);
}


int ifc_accept(int s, int adtfd){
  return NACL_SYSCALL(ifc_accept)(s, adtfd);
}

int ifc_connect(int s, struct sockaddr *addr, unsigned int len, int adtfd){
  return NACL_SYSCALL(ifc_connect)(s, (uint32_t) addr, len, adtfd);
}

int ifc_enqueue(int type, int sockfd, int adtfd){
  return NACL_SYSCALL(ifc_enqueue)(type, sockfd, adtfd);
}

int ifc_dequeue(){
  return NACL_SYSCALL(ifc_dequeue)();
}


// allocate memory in trusted area
int create_adt(int size) {
  int adtfd = 0;
  NACL_SYSCALL(create_adt)(&adtfd, size);
  return adtfd; 
}


int get_adt_entry(int adtfd, void *buf, size_t len) {

  return NACL_SYSCALL(get_adt_entry)(adtfd, buf, len, NON_SENSITIVE_ENTRY);
}

int get_adt_security_entry(int adtfd, void *buf, size_t len) {
  return NACL_SYSCALL(get_adt_entry)(adtfd, buf, len, SENSITIVE_ENTRY);
}

int add_adt_entry(int adtfd, void *buf, size_t len){
  return NACL_SYSCALL(add_adt_entry)(adtfd, buf, len);
}

int copy_adt_security_entry(int src_adtfd, int dst_adtfd){
  return 0; 
}


///////////////

int receive_adt(int s, void *buf, size_t len){
  return NACL_SYSCALL(receive_adt)(s, buf,  len);
}

int propagate_adt(int s, const void *buf, size_t len){
  return NACL_SYSCALL(propagate_adt)(s, buf, len);
}


void custom(){
  NACL_SYSCALL(custom)();
}

void custom_print(int prv){
  NACL_SYSCALL(custom_print)(prv);
}

#define NS_INADDRSZ        4

static int inet_pton4 (const char *src, const char *end, unsigned char *dst)
{
  int saw_digit, octets, ch;
  unsigned char tmp[NS_INADDRSZ], *tp;
  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;

  while (src < end)
  {
    ch = *src++;
    if (ch >= '0' && ch <= '9')
    {
      unsigned int new = *tp * 10 + (ch - '0');
      if (saw_digit && *tp == 0)
        return 0;
      if (new > 255)
        return 0;
      *tp = new;
      if (! saw_digit)
      {
        if (++octets > 4)
          return 0;
        saw_digit = 1;
      }
    }
    else if (ch == '.' && saw_digit)
    {
      if (octets == 4)
        return 0;
      *++tp = 0;
      saw_digit = 0;
    }
    else
      return 0;
  }
  if (octets < 4)
    return 0;

  for (int i=0; i<NS_INADDRSZ; i++){
    dst[i] = tmp[i];
  }
  return 1;
}

int
__inet_pton_length (int af, const char *src, size_t srclen, void *dst)
{
  switch (af)
  {
    case AF_INET:
      return inet_pton4 (src, src + srclen, dst);
    default:
      return -1;
  }
}


int inet_pton (int af, const char *src, int srclen, void *dst) {
  return __inet_pton_length (af, src, srclen, dst);
}
