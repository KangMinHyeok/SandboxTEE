/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFF_LEN 5000

char buffer_in[BUFF_LEN];
char buffer_out[BUFF_LEN];

int send_profile(int sockfd, int len) {
	int n;

	memcpy(buffer_out, &len, sizeof(len));
	memcpy(buffer_out+sizeof(len), buffer_in, len+1);

	n = write(sockfd, buffer_out, sizeof(len) + len + 1);
	if (n < 0)
		return n;

	return n;
}

int main(int argc, char *argv[]) {
	
	struct sockaddr_in address;
	int sockfd = 0;
	struct sockaddr_in serv_addr;
	int port, profile_num = 1;
	char *ip;

	int n, len;

	int ad_id;

	if (argc != 3) {
		printf("Usage: ./client [SSP ip] [SSP port]\n ex) ./client 147.46.215.133 8888\n");
		exit(-1);
	} else {
		ip = argv[1];
		port = atoi(argv[2]);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		exit(-1);
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(ip);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Connection failed\n");
		exit(-1);
	}


	memset(buffer_in, '0', BUFF_LEN);
	memset(buffer_out, '0', BUFF_LEN);

	srand(time(NULL));

	int gender = rand() %2;
	int age = rand()&8 + 2;
	int location = rand()%16 + 10;
	int interests = rand()%512 + 20;

	char *p = buffer_in;
	memcpy(p, &gender, sizeof(int)); p+= sizeof(int);
	memcpy(p, &age, sizeof(int)); p+= sizeof(int);
	memcpy(p, &location, sizeof(int)); p+= sizeof(int);
	memcpy(p, &interests, sizeof(int)); p+= sizeof(int);

	// read input file
	// while (fgets(buffer_in, sizeof(buffer_in), input_fp) != NULL) {

		printf("profile: %s", buffer_in);
		// send profile one by one
		n = send_profile(sockfd, p-buffer_in);
		if (n < 0) {
			printf("Write error\n");
			goto exit;
		}

		n = read(sockfd, buffer_in, BUFF_LEN);
		if (n < 0) {
			printf("Read error\n");
			goto exit;
		}

		memcpy(&len, buffer_in, sizeof(len));
		memcpy(&ad_id, buffer_in+sizeof(len), len);

		/*
		n = read(sockfd, &len, sizeof(len));
		if (n < 0) {
			printf("Read error\n");
			goto exit;
		}
		n = 0;
		while (n < len)
			n += read(sockfd, buffer_in, BUFF_LEN);

		if (n < 0) {
			printf("Read error\n");
			goto exit;
		}
		*/
		printf("Accepted AD id: %d\n\n", ad_id);

	//}

exit:
	close(sockfd);
	return 0;
}
