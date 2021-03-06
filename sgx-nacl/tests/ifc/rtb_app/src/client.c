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

#define BUFF_LEN 5000

char buffer_in[BUFF_LEN];
char buffer_out[BUFF_LEN];

int send_profile(int sockfd) {
	int n;

	int len = strlen(buffer_in);
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

	FILE *input_fp = NULL;
	int n, count, len;

	double bid;
	char *ad_id;

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

	input_fp = fopen("../data/test_input.txt", "r");
	if (input_fp == NULL) {
		printf("Cannot read test_input.txt\n");
		exit(-1);
	}

	memset(buffer_in, '0', BUFF_LEN);
	memset(buffer_out, '0', BUFF_LEN);

	// read input file
	count = 0;
	while (fgets(buffer_in, sizeof(buffer_in), input_fp) != NULL) {
		if (count > profile_num-1)
			break;

		printf("profile: %s", buffer_in);
		// send profile one by one
		n = send_profile(sockfd);
		if (n < 0) {
			printf("Write error\n");
			goto exit;
		}

		n = read(sockfd, buffer_in, BUFF_LEN);
		if (n < 0) {
			printf("Read error\n");
			goto exit;
		}

		memcpy(&bid, buffer_in, sizeof(bid));
		memcpy(&len, buffer_in+sizeof(bid), sizeof(len));
		printf("%d\n", len);
		ad_id = (char *)malloc(len * sizeof(char)+1);
		memcpy(ad_id, buffer_in+sizeof(bid)+sizeof(len), len+1);
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
		printf("Accepted Bid %.9f - AD id: %s\n\n", bid, ad_id);

		count++;
	}
	if (count < profile_num-1)
		printf("test_input.txt needs more profiles!\n");
	

exit:
	fclose(input_fp);
	close(sockfd);
	return 0;
}
