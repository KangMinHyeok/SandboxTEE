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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ifc.h>

#define BUFF_LEN 4096


char dummy[65536<<8];

void run_write(int size, char *filename, int buff_len, int lognum){

  int count = size/buff_len;

  if (count == 0) {
    count = 1;
    buff_len = size;
  }
  count = 1;
  int fd = open(filename, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

  custom_print(lognum);

  int ret;
  ret = write(fd, dummy, size);

  if (ret != size)
    printf("error while writing %s [ret: %d, count: %d] \n", filename, ret, count);
    
  custom_print(lognum);

  close(fd);
}

void run_read(int size, char *filename, int buff_len, int lognum){

 
  int count = size/buff_len;
  count = 1;
  if (count == 0) {
    count = 1;
    buff_len = size;
  }

  int fd = open(filename, O_RDWR);

  custom_print(lognum);

  int ret;
  ret = read(fd, dummy, size);

  if (ret != size)
    printf("error while reading %s [ret: %d, count: %d] \n", filename, ret, count);
  
  custom_print(lognum);
  
  close(fd);
}


int main(int argc, char *argv[]) {

  int buff_len = 1000;

  run_write(4096<<8,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len, 11);
  run_write(16384<<8,   "/home/mkpark/pave/depot_tools/native_client/write_100000", buff_len, 12);
  run_write(65536<<8, "/home/mkpark/pave/depot_tools/native_client/write_10000000", buff_len, 13);

  run_read (4096<<8,     "/home/mkpark/pave/depot_tools/native_client/read_10000000", buff_len, 21);
  run_read (16384<<8,   "/home/mkpark/pave/depot_tools/native_client/read_10000000", buff_len, 22);
  run_read (65536<<8, "/home/mkpark/pave/depot_tools/native_client/read_10000000", buff_len, 23);


}
