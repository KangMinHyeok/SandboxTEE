/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/


#include "util.h"

ADTFD pack_send_data(char *buffer, int len){

  char adt_buffer[ADT_SIZE];

  if (len - sizeof(int) > ADT_ENTRY_SIZE) {
    printf("send size overflow %d\n", len);
  }

  memset(adt_buffer, 0, ADT_SIZE);

  memcpy(adt_buffer, &len, sizeof(int));
  memcpy(adt_buffer + sizeof(int), buffer, len);

  ADTFD adt = create_adt(ADT_SIZE);

  if (!adt)
    printf("adt not allocated\n");
  add_adt_entry(adt, adt_buffer, len+sizeof(int));

  return adt;
}

int unpack_recv_data(ADTFD adt, char *buffer){

  char adt_buffer[ADT_SIZE];
  int len = 0;

  memset(adt_buffer, 0, ADT_SIZE);
  get_adt_entry(adt, adt_buffer, ADT_SIZE);

  memcpy(&len, adt_buffer, sizeof(int));
  memcpy(buffer, adt_buffer + sizeof(int), len);

  return len;
}


void send_data(int sockfd, char *buffer, int len) {

  char adt_buffer[ADT_SIZE];
  if (len - sizeof(int) > ADT_ENTRY_SIZE)
    printf("send size overflow\n");

  memset(adt_buffer, 0, ADT_SIZE);
  memcpy(adt_buffer, &len, sizeof(int));
  memcpy(adt_buffer + sizeof(int), buffer, len);

  ADTFD adt = create_adt(ADT_SIZE);
  add_adt_entry(adt, adt_buffer, len+sizeof(int));
  len = ifc_enqueue(QUEUE_ENTRY_SEND, sockfd, adt);

	// propagate_adt(sockfd, &len, sizeof(len));
	// propagate_adt(sockfd, buffer, len);
  close(adt);
}

int read_data(int sockfd, char *buffer) {
	int len;
  char adt_buffer[ADT_SIZE];
  
  ADTFD adt = create_adt(ADT_SIZE);
  len = ifc_enqueue(QUEUE_ENTRY_RECV, sockfd, adt);

  memset(adt_buffer, 0, ADT_SIZE);
  get_adt_entry(adt, adt_buffer, ADT_SIZE);

  memcpy(&len, adt_buffer, sizeof(int));
  memcpy(buffer, adt_buffer + sizeof(int), len);

  close(adt);

  return len;
/*
	receive_adt(sockfd, &len, sizeof(int));
	while (n < len) 
		n += receive_adt(sockfd, buffer + n, sizeof(buffer));
	return len;
  */
}


int connect_remote(int remote_port, ADTFD adt) {

	int sockfd;
	struct sockaddr_in serv_addr;

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = remote_port;
	serv_addr.sin_addr.s_addr = 16777343; 

	sockfd = ifc_socket();

	ifc_connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr), adt);

	return sockfd;
}

