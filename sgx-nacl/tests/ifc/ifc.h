

#include <sys/types.h>

#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* wolfSSL */
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>

#include "common.h"

#define QUEUE_ENTRY_SEND 0
#define QUEUE_ENTRY_RECV 1
#define SENSITIVE_ENTRY 0
#define NON_SENSITIVE_ENTRY 0

typedef unsigned char * ADTFD; 
#define create_adt malloc

int custom();
int ifc_bind  (int s, struct sockaddr *addr, unsigned int addrlen);
int ifc_accept(int s, ADTFD adtfd);
int ifc_connect(int s, struct sockaddr *addr, unsigned int len, ADTFD adtfd);
int ifc_enqueue(int type, int sockfd, ADTFD adtfd);
int ifc_dequeue();
int get_adt_entry(ADTFD adtfd, void *buf, size_t len);
int get_adt_security_entry(ADTFD adtfd, void *buf, size_t len) ;
int add_adt_entry(ADTFD adtfd, void *buf, size_t len);
int custom_print(int prv);

