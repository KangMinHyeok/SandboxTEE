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

#include "common.h"

void send_data(int sockfd, char *buffer, int len);
int read_data(int sockfd, char *buffer);
int connect_remote(int remote_port, ADTFD adt) ;
ADTFD pack_send_data(char *buffer, int len);
int unpack_recv_data(ADTFD adt, char *buffer);
