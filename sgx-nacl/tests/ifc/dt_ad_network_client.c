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

#include <fcntl.h>

#include <ifc.h>


int main(int argc, char *argv[]) {
	
	//struct sockaddr_in address;
	int sockfd;
	struct sockaddr_in serv_addr;
	//int port;
	int profile_num = 1;
	//char *ip;

	FILE *input_fp = NULL;
	int n, count, len;

	double bid;
	char *ad_id;

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = 5000;
	serv_addr.sin_addr.s_addr = 16777343;

	sockfd = ifc_socket();


	ifc_connect(sockfd, (uint32_t) &serv_addr, sizeof(serv_addr));

	input_fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/test_input.txt", "r");
	if (input_fp == NULL) {
		printf("Cannot read test_input.txt\n");
		return 0;
	}

	return 0;

	int BUFF_LEN = 1024;
	char buffer_in[BUFF_LEN];
	memset(buffer_in, '0', sizeof(buffer_in));
	
	count = 0;
	while (fgets(buffer_in, sizeof(buffer_in), input_fp) != NULL) {
		if (count > profile_num-1)
			break;

		printf("profile: %s", buffer_in);
		break;
		// send profile one by one
		if (n < 0) {
			printf("Write error\n");
			goto exit;
		}

		if (n < 0) {
			printf("Read error\n");
			goto exit;
		}

		memcpy(&bid, buffer_in, sizeof(bid));
		memcpy(&len, buffer_in+sizeof(bid), sizeof(len));
		printf("%d\n", len);
		ad_id = (char *)malloc(len * sizeof(char)+1);
		memcpy(ad_id, buffer_in+sizeof(bid)+sizeof(len), len+1);

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
		printf("Accepted Bid %.9f - AD id: %s\n\n", bid, ad_id);

		count++;
	}
	if (count < profile_num-1)
		printf("test_input.txt needs more profiles!\n");
	

exit:
	fclose(input_fp);
	return 0;
}
