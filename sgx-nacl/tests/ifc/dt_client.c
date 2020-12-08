/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "client_common.h"


int main(int argc, char *argv[]) {
	
	int sockfd;
	struct sockaddr_in serv_addr;
	char *ip = "127.0.0.1";

	char buffer[BUFF_LEN];

	int gender, age, location, interests;
	int product_id = 0;	


	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = 5000;
	serv_addr.sin_addr.s_addr = inet_addr(ip);


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		exit(-1);
	}

	gettimeofday(&before , NULL);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Connection failed\n");
		exit(-1);
	}

	memset(buffer, '0', sizeof(buffer));

	srand(time(NULL));

	gender = rand() % 2;
	age = rand() % 8 + 2;
	location = rand() % 16 + 10;
	interests = rand() % 512 + 20;

	char *p = buffer;
	memcpy(p, &gender, sizeof(int));
	p += sizeof(int);
	memcpy(p, &age, sizeof(int));
	p += sizeof(int);
	memcpy(p, &location, sizeof(int));
	p += sizeof(int);
	memcpy(p, &interests, sizeof(int));
	p += sizeof(int);

	printf("gender: %d, age: %d, location: %d, interests: %d\n", gender, age, location, interests);
	send_data(sockfd, buffer, p-buffer);


	read_data(sockfd, buffer);
	memcpy(&product_id, buffer, sizeof(int)); 

	printf("product_id: %x\n", product_id);
	printf("Aid id: %d\n", product_id);
	

	close(sockfd);
	
  gettimeofday(&after , NULL);

	printf("Total time elapsed (us), %.0lf\n" , time_diff(before , after) ); 

	return 0;
}
