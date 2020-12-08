/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/


#include <ifc.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

int main(int argc, char* argv[]) {

  struct sockaddr_in serv_addr; 
  memset(&serv_addr, '0', sizeof(serv_addr));


  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = 0;
  serv_addr.sin_port = 5000; 

  int sock = ifc_socket();
  int csock;

  ifc_bind  (sock, (uint32_t) &serv_addr, sizeof(serv_addr));
  csock = ifc_accept(sock);

  char sendBuff[1025];
  memset(sendBuff, '0', sizeof(sendBuff)); 

  receive_adt(csock, sendBuff, 12);

  printf("%s\n", sendBuff);
  
  return 0;
}
