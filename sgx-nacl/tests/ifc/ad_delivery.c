/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFF_LEN 20000 

char buffer[BUFF_LEN];
char profile[BUFF_LEN];


int main(int argc, char *argv[]) {

	int serv_sockfd, client_sockfd;
	struct sockaddr_in serv_addr, client_addr;
	int client_len;
	int port, n, len;


	int rating, product_id;
	FILE *fp = NULL;
	char *line;
	int i, j;

	if (argc != 2) {
		printf("Usage: ./server [Port]\n ex) ./server 8888\n");
		exit(-1);
	} else{
		port = atoi(argv[1]);
	}


	// read user profile
	if ((serv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("Socket creation failed\n");
		exit(-1);
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	if (bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Bind error\n");
		goto exit2;
	}

	if (listen(serv_sockfd, 5) < 0) {
		printf("Listen error\n");
		goto exit2;
	}

	client_len = sizeof(client_addr);
	if ((client_sockfd = accept(serv_sockfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
		printf("Accept error\n");
		goto exit2;
	}

	n = send_request(client_sockfd, buffer, BUFF_LEN);

exit1:
	close(client_sockfd);
exit2:
	close(serv_sockfd);

	return 0;
	
}