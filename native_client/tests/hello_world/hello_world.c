/*
 * Copyright 2008 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * It can't get much simpler than this (uh, except for noop.c).
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void hello_world(void) {
  printf("Hello, World!\n");
  int f = open("/home/hmlee/SandboxTEE/native_client/tests/hello_world/testdata256.txt", O_RDWR, 0644);
  if (f < 0) {
    printf("hello_world - File Open Fail\n");
  } else {
  	printf("hello_world - File Open Success\n");
  }

  char *str = "aaaaaa";
  write(f, str, strlen(str));
  close(f);
}

int main(int argc, char* argv[]) {
  hello_world();
  return 0;
}
