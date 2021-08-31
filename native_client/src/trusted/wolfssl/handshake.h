#ifndef __XCALL_HANDSHAKE__
#define __XCALL_HANDSHAKE__

#include "native_client/src/trusted/wolfssl/options.h"
#include "native_client/src/trusted/wolfssl/ssl.h"
#include "native_client/src/trusted/wolfssl/types.h"
#include "native_client/src/trusted/wolfssl/settings.h"
#include "native_client/src/trusted/wolfssl/coding.h"


WOLFSSL *ssl_handshake_new(char *ip, int port, WOLFSSL_CTX* ctx) ;
WOLFSSL *ssl_handshake(char *ip, int port, WOLFSSL_CTX* ctx) ;
WOLFSSL *ssl_initiate(char *ip, int port, WOLFSSL_CTX* ctx) ;

int cert_verify_callback(int preverify, WOLFSSL_X509_STORE_CTX* store) ;
extern uint8_t ca_cert[1501];

#endif
