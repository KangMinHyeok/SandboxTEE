/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

all: client.o ad_network.o
	gcc -o client client.c
	gcc -o ad_network ad_network.c

clean:
	rm client ad_network *.o
