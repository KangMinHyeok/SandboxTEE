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
#include <math.h>

#define BUFF_LEN 5000

char buffer_in[BUFF_LEN];
char buffer_out[BUFF_LEN];
double *weights[5];
char *ad_ids[5] = {"ad_1458", "ad_3358", "ad_3386", "ad_3427", "ad_3476"};

float sigmoid(float x) {
	float exp_value, return_value;

	exp_value = exp((double) -x);
	return_value = 1 / (1 + exp_value);

	return return_value;
}

double ortb1(double pctr, double lambda, double c) {
	double bid;

	bid = (c/lambda)*pctr + c*c;
	bid = sqrt(bid);
	bid -= c;

	printf("Bid(ortb1): %10.9f\n", bid);
	return bid;
}

double ortb2(double pctr, double lambda, double c) {
	double bid, tmp;

	bid = (c*c)*(lambda*lambda) + (pctr*pctr);
	bid = pctr + sqrt(bid);
	bid = bid / (c*lambda);

	tmp = 1/bid;

	bid = pow(bid, (double)1/3);
	tmp = pow(tmp, (double)1/3);

	bid = c*(bid - tmp);

	printf("Bid(ortb2): %10.9f\n", bid);
	return bid;
}


double calc_pctr(char *delim, char *buffer, double *weight) {
	double pctr = 0;
	char *line;
	int feat;

	line = strtok(buffer, delim);
	line = strtok(NULL, delim);
	line = strtok(NULL, delim);

	while (1) {
		line = strtok(NULL, delim);
		if (line == NULL) break;
		line = strtok(NULL, delim);
		if (line == NULL) break;

		feat = atof(line);
		pctr += weight[feat];
	}

	pctr = sigmoid(pctr);

	return pctr;
}

int find_ad(char *profile, double *result) {
	char *delim = " :";
	int weight_num = 5;
	int selected_ad = -1;
	int i;
	double pctr, max = -1;
	char *tmp_profile;
	tmp_profile = (char *)malloc(strlen(profile)*sizeof(char)+1);

	for (i = 0; i < weight_num; i++) {
		memcpy(tmp_profile, profile, strlen(profile)+1);
		pctr = calc_pctr(delim, tmp_profile, weights[i]);
		if (pctr > max) {
			max = pctr;
			selected_ad = i;
		}
	}
	memcpy(result, &max, sizeof(max));
	return selected_ad;
}

void read_weight_aux(FILE *fp, double *weight) {
	char *line;
	int tmp_idx;

	while (fgets(buffer_in, BUFF_LEN, fp) != NULL) {
		line = strtok(buffer_in, "	");
		tmp_idx = atoi(line);

		line = strtok(NULL, "	");
		weight[tmp_idx] = atof(line);
	}
}

int read_weights(int feat_num) {
	FILE *fp = NULL;
	int i, weight_num = 5;

	for (i = 0; i < weight_num; i++) {
		weights[i] = (double *)malloc(sizeof(double)*(feat_num+1));
		memset(weights[i], 0, feat_num+1);
	}

	fp = fopen("../data/weight_1458.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[0]);
	fclose(fp);

	fp = fopen("../data/weight_3358.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[1]);
	fclose(fp);

	fp = fopen("../data/weight_3386.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[2]);
	fclose(fp);

	fp = fopen("../data/weight_3427.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[3]);
	fclose(fp);

	fp = fopen("../data/weight_3476.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[4]);
	fclose(fp);


	return 1;
}

int main(int argc, char *argv[]) {

	int serv_sockfd, client_sockfd;
	struct sockaddr_in client_addr, serv_addr;
	int client_len, n, len, result, i;
	int port;
	int feat_num, tmp_idx, algorithm, count, selected_ad;
	double pctr, lambda, c, bid;

	if (argc != 4) {
		printf("Usage: ./dsp [PORT] [# of feature] [Bid algorithm]\n ex) ./dsp 8888 10000 1\n");
		exit(-1);
	} else {
		port = atoi(argv[1]);
		feat_num = atoi(argv[2]);
		algorithm = atoi(argv[3]);
	}

	if (read_weights(feat_num) < 0) {
		printf("Read weight error\n");
		exit(-1);
	}

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

	lambda = 5.2 * pow(10, -7);
	c = 50;

	memset(buffer_in, '0', BUFF_LEN);
	memset(buffer_out, '0', BUFF_LEN);
	char *delim = " :";
	count = 0;
	while (count < 1) {

		n = read(client_sockfd, buffer_in, BUFF_LEN);
		if (n < 0) {
			printf("Read error\n");
			goto exit1;
		}
		memcpy(&len, buffer_in, sizeof(len));
		memcpy(buffer_out, buffer_in+sizeof(len), len+1);
		
		printf("profile: %s", buffer_out);
		
		selected_ad = find_ad(buffer_out, &pctr);

		printf("Selected ad campaign: %s, ", ad_ids[selected_ad]);

		//pctr = calc_pctr(delim, buffer_out, weights[0]);
		if (algorithm == 0)
			bid = ortb1(pctr, lambda, c);
		else
			bid = ortb2(pctr, lambda, c);

		memcpy(buffer_out, &bid, sizeof(bid));
		len = strlen(ad_ids[selected_ad]);
		memcpy(buffer_out+sizeof(bid), &len, sizeof(len));
		memcpy(buffer_out+sizeof(bid)+sizeof(len), ad_ids[selected_ad], len+1);
		n = write(client_sockfd, buffer_out, sizeof(bid)+sizeof(len)+len+1);
		if (n < 0) {
			printf("Write error\n");
			goto exit1;
		}

		printf("Done!\n");

		count++;
	}

exit1:
	for (i = 0; i < 5; i++)
		free(weights[i]);
	close(client_sockfd);
exit2:
	close(serv_sockfd);


	return 0;
}
