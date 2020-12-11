/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <ifc.h>
#include "util.h"


int main(int argc, char *argv[]) {

	int BUFF_LEN = 4096;
	char buffer[BUFF_LEN];
	int serv_sockfd, client_sockfd;
	struct sockaddr_in serv_addr;
	int len;
  int ret = 0;

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = 0;
	serv_addr.sin_port = 9001;

	serv_sockfd = ifc_socket();

	ret = ifc_bind  (serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (ret < 0) {
    printf("bind error\n");
    exit(3);
  }

  ADTFD adt1 = create_adt(ADT_SIZE);
  custom_print(77);
	client_sockfd = ifc_accept(serv_sockfd, adt1);
  custom_print(105);
  if (client_sockfd < 0){
    close(serv_sockfd);
    exit(2);
  }
	len = unpack_recv_data(adt1, buffer);

	send_data(client_sockfd, buffer, len);

  // receive result
	int send_size;
  send_size = read_data(client_sockfd, buffer);

  close(client_sockfd);
  close(serv_sockfd);

  custom_print(106);
	return 0;
}
