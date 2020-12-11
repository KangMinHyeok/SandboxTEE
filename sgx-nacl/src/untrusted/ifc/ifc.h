#ifndef __IFC__
#define __IFC__ 1

#include <sys/types.h>

#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <stdint.h>


#define AF_INET 2
#define QUEUE_ENTRY_SEND 0
#define QUEUE_ENTRY_RECV 1
#define SENSITIVE_ENTRY 0
#define NON_SENSITIVE_ENTRY 0

typedef int ADTFD;

typedef unsigned short int sa_family_t ;

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

struct sockaddr {
  sa_family_t sa_family;
  char sa_data[128-sizeof(unsigned short)];
};

void custom();
void custom_print(int prv);

int inet_pton (int af, const char *src, int srclen, void *dst) ;

int ifc_socket(void);
int ifc_bind  (int s, struct sockaddr *addr, unsigned int addrlen);
ADTFD ifc_accept(int s, ADTFD adtfd);
ADTFD ifc_connect(int s, struct sockaddr *addr, unsigned int len, ADTFD adtfd);
int ifc_enqueue(int type, int sockfd, ADTFD adtfd);
int ifc_dequeue();
int create_adt(int size);
int get_adt_entry(ADTFD adtfd, void *buf, size_t len);
int get_adt_security_entry(ADTFD adtfd, void *buf, size_t len) ;
int add_adt_entry(ADTFD adtfd, void *buf, size_t len);
int copy_adt_security_entry(int src_adtfd, int dst_adtfd);

int receive_adt(int s, void *buf, size_t len);
int propagate_adt(int s, const void *buf, size_t len);
#endif
