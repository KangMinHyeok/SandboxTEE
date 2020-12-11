/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include <ifc.h>

int main(int argc, char* argv[]) {

  struct sockaddr_in serv_addr; 
  memset(&serv_addr, '0', sizeof(serv_addr)); 

  char sendBuff[10] = {'h', 'e', 'l', 'l', 'o', '\0'};
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = 5000; 
  serv_addr.sin_addr.s_addr = 16777343;

  int sock = ifc_socket();


  ifc_connect(sock, (uint32_t) &serv_addr, sizeof(serv_addr));



  propagate_adt(sock, sendBuff, 6);

  return 0;
}
