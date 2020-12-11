/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "client_common.h"

int main(int argc, char *argv[]) {

	char buffer[BUFF_LEN];

	int sockfd = 0;
	struct sockaddr_in serv_addr;
	char *ip = "127.0.0.1";

	FILE *input_fp = NULL;

	double bid = 0;
	char *ad_id = NULL;
	int len;

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = 5000;
	serv_addr.sin_addr.s_addr = inet_addr(ip);


	// create socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation error\n");
		exit(-1);
	}

	// connect to server
	gettimeofday(&before , NULL);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Connection failed\n");
		exit(-1);
	}

	input_fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/rtb_app/data/test_input.txt", "r");
	if (input_fp == NULL) {
		printf("Cannot read test_input.txt\n");
		exit(-1);
	}

	memset(buffer, 0, BUFF_LEN);

	// read input file
	if (fgets(buffer, sizeof(buffer), input_fp) == NULL) {
		printf("Read test_input.txt Error\n");
		return 0;
	}
	
	printf("profile: %s", buffer);

	// send and read data
	send_data(sockfd, buffer, strlen(buffer) + 1);

	read_data(sockfd, buffer);

	memcpy(&bid, buffer, sizeof(bid));
	memcpy(&len, buffer+sizeof(bid), sizeof(len));
	ad_id = (char *)malloc(len * sizeof(char));
	memcpy(ad_id, buffer+sizeof(bid)+sizeof(len), len);

	printf("Accepted Bid %.9f - AD id: %s\n\n", bid, ad_id);

	close(sockfd);
	fclose(input_fp);
	gettimeofday(&after , NULL);

	printf("Total time elapsed (us), %.0lf\n" , time_diff(before , after) ); 

	return 0;
}
