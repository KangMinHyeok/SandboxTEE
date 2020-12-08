/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <time.h>
#include "ifc.h"


WOLFSSL_CTX* ctx[100];

WOLFSSL*     ssl[100];
   

#define CA_CERT "/home/mkpark/cacert.pem"
#define CERT_FILE "/home/mkpark/cert.pem"
#define KEY_FILE  "/home/mkpark/private.pem"


int custom() {
}

int ifc_socket(){

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    return -1;

  wolfSSL_Init();
//  wolfSSL_Debugging_ON();

  return sock;
}


int ifc_bind  (int s, struct sockaddr *addr, unsigned int addrlen){

  int nSockOpt = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));

  int state = bind(s, (struct sockaddr *) addr, sizeof(struct sockaddr_in));
  if (state == -1)
  {
    perror("bind error : ");
    exit(0);
  }

  state = listen(s, 5);
  if (state == -1)
  {
    perror("listen error : ");
    exit(0);
  }
}


int ifc_accept(int s, ADTFD adtfd){


  struct sockaddr_in clientaddr;
  int state, client_len;
  client_len = sizeof(clientaddr);

  int client_sockfd = accept(s, (struct sockaddr *)&clientaddr,
      &client_len);
  if (client_sockfd == -1)
  {
    perror("Accept error : ");
    exit(0);
  }

  if ((ctx[client_sockfd] = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL) {
    fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
    exit(2);
  }

  wolfSSL_CTX_set_verify(ctx[client_sockfd], SSL_VERIFY_NONE, NULL);

  if (wolfSSL_CTX_load_verify_locations(ctx[client_sockfd], CA_CERT, NULL) != SSL_SUCCESS)
    perror("load ca error");

    /* Load server certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_certificate_file(ctx[client_sockfd], CERT_FILE, SSL_FILETYPE_PEM)
        != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
                CERT_FILE);
        return -1;
    }

    /* Load server key into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_PrivateKey_file(ctx[client_sockfd], KEY_FILE, SSL_FILETYPE_PEM)
        != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
                KEY_FILE);
        return -1;
    }


  if ((ssl[client_sockfd] = wolfSSL_new(ctx[client_sockfd])) == NULL) {
    fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
    exit(1);
  }


  wolfSSL_set_fd(ssl[client_sockfd], client_sockfd);

  if ( wolfSSL_accept(ssl[client_sockfd]) < 0){
    exit(3);
  }

  custom_print(100);

  int total, count;
  total = count = ADT_SIZE;
  int len = 0; // wolfSSL_read(ssl[client_sockfd], adtfd, ADT_SIZE);

    while (count > 0){
      len = wolfSSL_read(ssl[client_sockfd], adtfd+ (total-count), count);

      if (len < 0)
        return len;

      count -= len;

    }


//  int len = read(client_sockfd, adtfd, ADT_SIZE);
  // printf("accept: %d\n", len);
  return client_sockfd;
}

int ifc_connect(int s, struct sockaddr *addr, unsigned int len, ADTFD adtfd){

  if ( connect(s, addr, len) < 0)
    return -1;

  if ((ctx[s] = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
    fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
    exit(2);
  }

  wolfSSL_CTX_set_verify(ctx[s], SSL_VERIFY_NONE, NULL);

  if (wolfSSL_CTX_load_verify_locations(ctx[s], CA_CERT, NULL) != SSL_SUCCESS)
    perror("load ca error");

    /* Load server certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_certificate_file(ctx[s], CERT_FILE, SSL_FILETYPE_PEM)
        != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
                CERT_FILE);
        return -1;
    }

    /* Load server key into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_PrivateKey_file(ctx[s], KEY_FILE, SSL_FILETYPE_PEM)
        != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
                KEY_FILE);
        return -1;
    }


  if ((ssl[s] = wolfSSL_new(ctx[s])) == NULL) {
    fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
    exit(1);
  }


  wolfSSL_set_fd(ssl[s], s);

  if (wolfSSL_connect(ssl[s]) != SSL_SUCCESS) {
    fprintf(stderr, "ERROR: failed to connect to wolfSSL\n");
    exit(3);
  }

  int count, total;
  total = count = ADT_SIZE;

  ADTFD adt = create_adt(ADT_SIZE);

  while (count > 0){  
    len = wolfSSL_write(ssl[s], adtfd + (total-count), count);

    if (len < 0)
      return len;
    count-=len;
  }

//  len = write(s, adtfd, ADT_SIZE);
  // printf("connect %d\n", len);

}


int ifc_enqueue(int type, int sockfd, ADTFD adtfd){
  int count = ADT_SIZE, len = 0;
  if (type == QUEUE_ENTRY_SEND){

    while (count > 0){  
      len = wolfSSL_write(ssl[sockfd], adtfd + (ADT_SIZE-count), count);

      if (len < 0)
        return len;
      count-=len;
    }
    // write(sockfd, adtfd, ADT_SIZE);
  }
  else{

    while (count > 0){
      len = wolfSSL_read(ssl[sockfd], adtfd+ (ADT_SIZE-count), count);

      if (len < 0)
        return len;

      count -= len;

    }
    // read(sockfd, adtfd, ADT_SIZE);
  }
}


int ifc_dequeue(){
}

int get_adt_entry(ADTFD adtfd, void *buf, size_t len){
  memcpy(buf, adtfd, len);

}
int get_adt_security_entry(ADTFD adtfd, void *buf, size_t len) {
}

int add_adt_entry(ADTFD adtfd, void *buf, size_t len){
  memcpy(adtfd, buf, len);
}


int custom_print(int prv){
    struct timespec tv;

    clock_gettime(CLOCK_MONOTONIC,&tv);

    printf("[%d] tvsec: %lu\n", prv, tv.tv_sec*1000000000 + tv.tv_nsec);
 
}

