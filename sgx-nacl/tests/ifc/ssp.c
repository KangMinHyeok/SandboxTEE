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

#define BUFF_LEN 2048 

char buffer_in[BUFF_LEN];
char buffer_out[BUFF_LEN];


int main(int argc, char *argv[]) {
	int user_serv_sockfd, user_client_sockfd, adex_sockfd;
	struct sockaddr_in user_client_addr, user_serv_addr, adex_serv_addr;
	int user_client_len;
	char *adex_ip, *ad_id;
	int user_port, adex_port;
	int n, len, count = 0;
	double bid;

	if (argc != 4) {
		printf("Usage: ./ssp [User port] [AdExchange ip] [AdExchange port]\n ex) ./ssp 7777 147.46.215.133 8888\n");
		exit(-1);
	} else {
		user_port = atoi(argv[1]);
		adex_ip = argv[2];
		adex_port = atoi(argv[3]);
	}

	if ((user_serv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("User - Socket creation failed\n");
		exit(-1);
	}

	memset(&user_serv_addr, '0', sizeof(user_serv_addr));
	user_serv_addr.sin_family = AF_INET;
	user_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	user_serv_addr.sin_port = htons(user_port);

	if (bind(user_serv_sockfd, (struct sockaddr *)&user_serv_addr, sizeof(user_serv_addr)) < 0) {
		printf("Bind error\n");
		goto exit3; 
	}

	if (listen(user_serv_sockfd, 5) < 0) {
		printf("Listen error\n");
		goto exit3;
	}

	user_client_len = sizeof(user_client_addr);
	if ((user_client_sockfd = accept(user_serv_sockfd, (struct sockaddr *)&user_client_addr, &user_client_len)) < 0) {
		printf("Accept error\n");
		goto exit3;
	}

	if ((adex_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("Ad exchange - Socket creation failed\n");
		goto exit2;
	}

	memset(&adex_serv_addr, '0', sizeof(adex_serv_addr));
	adex_serv_addr.sin_family = AF_INET;
	adex_serv_addr.sin_port = htons(adex_port);
	adex_serv_addr.sin_addr.s_addr = inet_addr(adex_ip);

//	printf("%s %d\n", __FUNCTION__, __LINE__); fflush(stdout);
	if (connect(adex_sockfd, (struct sockaddr *)&adex_serv_addr, sizeof(adex_serv_addr)) < 0) {
		printf("Ad exchange - Connection failed\n");
		goto exit3;
	}

//	printf("%s %d\n", __FUNCTION__, __LINE__); fflush(stdout);
	while (count < 1) {
		// read profile from a user
//	printf("%s %d\n", __FUNCTION__, __LINE__); fflush(stdout);
		n = read_nonuser_data(user_client_sockfd, buffer_in, BUFF_LEN);
		if (n < 0) {
			printf("User - Read error\n");
			goto exit1;
		}
		printf("%d bytes read\n", n);
		fflush(stdout);
		/*
		memcpy(&len, buffer_in, sizeof(len));
		memcpy(buffer_out, buffer_in+sizeof(len), len+1);
		
		printf("Profile: %s", buffer_out);
		*/

		// send profile to an ad exchange
		n = extend_request(adex_sockfd, buffer_in, BUFF_LEN);
		if (n < 0) {
			printf("Ad exchange - Write error\n");
			goto exit1;
		}

		// read result from the ad exchange
		n = gather_response(adex_sockfd, buffer_in, BUFF_LEN);
		if (n < 0) {
			printf("Ad exchange - Read error\n");
			goto exit1;
		}

		/*
		memcpy(&bid, buffer_in, sizeof(bid));
		memcpy(&len, buffer_in+sizeof(bid), sizeof(len));
		memcpy(buffer_out, buffer_in+sizeof(len)+sizeof(bid), len+1);
		printf("Bid id: %s\n", buffer_out);
		*/

		// send result to the user
		n = send_response(user_client_sockfd, buffer_in, BUFF_LEN);
		if (n < 0) {
			printf("User - Write error\n");
			goto exit1;
		}

		count++;
	}



exit1:
	close(adex_sockfd);
exit2:
	close(user_client_sockfd);
exit3:
	close(user_serv_sockfd);

	return 0;

}
