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

void hello_world(void) {
  printf("Hello, World!\n");
	int f = open("/home/hmlee/SandboxTEE/native_client/tests/hello_world/testdata256.txt", O_CREAT, 0644);
  if (f < 0) {
    printf("open fail\n");
  } else {
  	printf("open success\n");
  }
  close(f);
}

int main(int argc, char* argv[]) {
  hello_world();
  return 0;
}
