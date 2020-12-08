/*
 * Copyright 2020 The SandboxTEE Authors. All rights reserved.
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

char buffer[BUFF_LEN];
char profile[BUFF_LEN];


int cmp_bids(double *bid_list, int dsp_num) {

	int i, winner = 0;
	double max = 0;

	for (i = 0; i < dsp_num; i++) {
		if (bid_list[i] > max) {
			max = bid_list[i];
			winner = i;
		}
	}
	return winner;
}

int send_profile(int sockfd, int len, char *buffer) {
	int n;
	n = extend_request(sockfd, buffer, BUFF_LEN);
	return n;
}

int connect_dsp(char* dsp_ip, int dsp_port) {

	int dsp_sockfd;
	struct sockaddr_in dsp_serv_addr;
	
	if ((dsp_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("DSP - Socket creation failed\n");
		return -1;
	}

	memset(&dsp_serv_addr, '0', sizeof(dsp_serv_addr));
	dsp_serv_addr.sin_family = AF_INET;
	dsp_serv_addr.sin_port = htons(dsp_port);
	dsp_serv_addr.sin_addr.s_addr = inet_addr(dsp_ip);

	if (connect(dsp_sockfd, (struct sockaddr *)&dsp_serv_addr, sizeof(dsp_serv_addr)) < 0) {
		printf("DSP - Connection failed\n");
		close(dsp_sockfd);
		return -1;
	}

	return dsp_sockfd;
}

int main(int argc, char *argv[]) {

	int ssp_serv_sockfd, ssp_client_sockfd, dsp_sockfd;
	struct sockaddr_in ssp_client_addr, ssp_serv_addr;
	int ssp_client_len;
	int ssp_port, dsp_port, tmp_port;
	int n, len, profile_len, i, dsp_num, winner, result, count;
	double *bid_list;
	char **ad_id_list;
	double highest_bid;
	int *fd_list;

	if (argc < 4) {
		printf("Usage: ./ad_exchange [SSP port] [DSP1 ip] ... [DSP port]\n ex) ./ad_exchange 7777 147.46.215.133 147.46.215.122 8888\n");
		exit(-1);
	} else {
		ssp_port = atoi(argv[1]);

		dsp_num = argc-3;
		dsp_port = atoi(argv[argc-1]);
	}

	printf("Number of DSPs: %d\n", dsp_num);

	if ((ssp_serv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("SSP - Socket creation failed\n");
		exit(-1);
	}
	
	memset(&ssp_serv_addr, '0', sizeof(ssp_serv_addr));
	ssp_serv_addr.sin_family = AF_INET;
	ssp_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ssp_serv_addr.sin_port = htons(ssp_port);

	if (bind(ssp_serv_sockfd, (struct sockaddr *)&ssp_serv_addr, sizeof(ssp_serv_addr)) < 0) {
		printf("Bind error\n");
		goto exit3;
	}

	if (listen(ssp_serv_sockfd, 5) < 0) {
		printf("Listen error\n");
		goto exit3;
	}

	ssp_client_len = sizeof(ssp_client_addr);
	if ((ssp_client_sockfd = accept(ssp_serv_sockfd, (struct sockaddr *)&ssp_client_addr, &ssp_client_len)) < 0) {
		printf("Accept error\n");
		goto exit3;
	}

	tmp_port = dsp_port;
	fd_list = (int *)malloc(sizeof(int)*(dsp_num));
	for (i=2; i < dsp_num+2; i++) {
		dsp_sockfd = connect_dsp(argv[i], tmp_port);
		if (dsp_sockfd < 0) {
			goto exit2;
		}
		fd_list[i-2] = dsp_sockfd; 
		tmp_port++;
	}

	memset(buffer, '0', BUFF_LEN);
	bid_list = (double *)malloc(sizeof(double)*(dsp_num));
	ad_id_list = (char **)malloc(dsp_num * sizeof(char *));
	count = 0;
	while (count < 1) {

		n = read_nonuser_data(ssp_client_sockfd, profile, BUFF_LEN);
		if (n < 0) {
			printf("SSP - Read error\n");
			goto exit1;
		}

				fflush(stdout);
		for (i = 0; i < dsp_num; i++) {
			dsp_sockfd = fd_list[i];

			n = send_profile(dsp_sockfd, profile_len+sizeof(profile_len)+1, profile);
			if (n < 0) {
				printf("Send profile error\n");
				goto exit1;
			}
				fflush(stdout);

			n = gather_response(dsp_sockfd, buffer, BUFF_LEN);
			if (n < 0) {
				printf("DSP - Read error %d\n", n);
				fflush(stdout);
				goto exit1;
			}
//			memcpy(&profile_len, profile, sizeof(profile_len));
//			printf("profile length: %d\n", profile_len); fflush(stdout);
//			memcpy(buffer, profile+sizeof(profile_len), profile_len+1);
//			fflush(stdout);
			
			//printf("buff: %s", buffer);
		
			memcpy(&bid_list[i], buffer, sizeof(&bid_list[i]));

			memcpy(&len, buffer+sizeof(&bid_list[i]), sizeof(len));
			ad_id_list[i] = (char *)malloc(len * sizeof(char));
			memcpy(ad_id_list[i], buffer+sizeof(&ad_id_list[i])+sizeof(len), len+1);

			printf("Id: %d, bid: %.10f, ad id: %s\n", i, bid_list[i], ad_id_list[i]);
		}

		winner = cmp_bids(bid_list, dsp_num);
		printf("Winner: %d, bid: %.9f, ad id: %s\n\n", winner, bid_list[winner], ad_id_list[winner]);

		memcpy(buffer, &bid_list[winner], sizeof(bid_list[winner]));
		len = strlen(ad_id_list[winner]);
		memcpy(buffer+sizeof(bid_list[winner]), &len, sizeof(len));
		memcpy(buffer+sizeof(bid_list[winner])+sizeof(len), ad_id_list[winner], len+1);

		n = send_response(ssp_client_sockfd, buffer, BUFF_LEN);
		
		count++;
	}

exit1:
	for (i = 0; i < dsp_num; i++)
		close(fd_list[i]);
		free(ad_id_list[i]);
	free(ad_id_list);
	free(bid_list);
exit2:
	close(ssp_client_sockfd);
exit3:
	close(ssp_serv_sockfd);

	return 0;
}
