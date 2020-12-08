#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <ifc.h>

#include "util.h"


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

	// printf("Bid(ortb1): %10.9f\n", bid);
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

	// printf("Bid(ortb2): %10.9f\n", bid);
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

int find_ad(char *profile, double *weights[], int profile_len, double *result) {
	char *delim = " :";
	int weight_num = 5;
	int selected_ad = -1;
	int i;
	double pctr, max = -1;
	char *tmp_profile;
	tmp_profile = (char *)malloc(profile_len*sizeof(char));

	for (i = 0; i < weight_num; i++) {
		memcpy(tmp_profile, profile, profile_len);
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
	int BUFF_LEN = 4096;
	char buffer[BUFF_LEN];

	while (fgets(buffer, BUFF_LEN, fp) != NULL) {
		line = strtok(buffer, "	");
		tmp_idx = atoi(line);

		line = strtok(NULL, "	");
		weight[tmp_idx] = atof(line);
	}
}

int read_weights(double *weights[]) {
	FILE *fp = NULL;

	fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/rtb_app/data/weight_1458.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[0]);
	fclose(fp);

	fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/rtb_app/data/weight_3358.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[1]);
	fclose(fp);

	fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/rtb_app/data/weight_3386.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[2]);
	fclose(fp);

	fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/rtb_app/data/weight_3427.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[3]);
	fclose(fp);

	fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/rtb_app/data/weight_3476.txt", "r");
	if (fp == NULL) {
		printf("Cannot open weight input\n");
		return -1;
	}
	read_weight_aux(fp, weights[4]);
	fclose(fp);

	return 1;
}

int main(int argc, char *argv[]) {

  custom_print(113);
	int FEAT_NUM = 10001;
	int WEIGHT_NUM = 5;
	int ALGO = 1;
	int BUFF_LEN = 4096;

	char buffer[BUFF_LEN];
	char buffer2[BUFF_LEN];
	int serv_sockfd, client_sockfd, analyzer_sockfd;
	struct sockaddr_in serv_addr;
	int len, i;
	int selected_ad;
	double pctr, lambda, c, bid;
  int ret = 0;

	double *weights[WEIGHT_NUM];
	char *ad_ids[5] = {"ad_1458", "ad_3358", "ad_3386", "ad_3427", "ad_3476"};
	
	
	for (i = 0; i < WEIGHT_NUM; i++) {
		weights[i] = (double *)malloc(sizeof(double)*(FEAT_NUM));
		memset(weights[i], 0, FEAT_NUM);
	}

	if (read_weights(weights) < 0) {
		printf("Read weight error\n");
		return 0;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = 0;
	serv_addr.sin_port = 7001;

	serv_sockfd = ifc_socket();
	ret = ifc_bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (ret < 0) {
    printf("bind error\n");
    exit(3);
  }

  ADTFD adt1 = create_adt(ADT_SIZE);
  custom_print(77);
	client_sockfd = ifc_accept(serv_sockfd, adt1);
  if (client_sockfd < 0){
    close(serv_sockfd);
    exit(2);
  }

	lambda = 5.2 * pow(10, -7);
	c = 50;

	memset(buffer, 0, sizeof(buffer));
	memset(buffer2, 0, sizeof(buffer2));
	
	// len = read_data(client_sockfd, buffer);
	len = unpack_recv_data(adt1, buffer);
	// printf("profile: %s", buffer);
	 
  ADTFD adt = pack_send_data(buffer, len); 
  analyzer_sockfd = connect_remote  (9001, adt);
  // send_data(analyzer_sockfd, buffer, len);  

  read_data(analyzer_sockfd, buffer2);
	
	selected_ad = find_ad(buffer, weights, len, &pctr);

	// printf("Selected ad campaign: %s, ", ad_ids[selected_ad]);

	if (ALGO == 0)
		bid = ortb1(pctr, lambda, c);
	else
		bid = ortb2(pctr, lambda, c);

	memcpy(buffer, &bid, sizeof(bid));
	len = strlen(ad_ids[selected_ad]) + 1;
	memcpy(buffer+sizeof(bid), &len, sizeof(len));
	memcpy(buffer+sizeof(bid)+sizeof(len), ad_ids[selected_ad], len);
	send_data(client_sockfd, buffer, sizeof(bid)+sizeof(len)+len);
  custom_print(200);

  // receive result
	int send_size;
  send_size = read_data(client_sockfd, buffer);
  send_data(analyzer_sockfd, buffer, send_size);

	for (i = 0; i < WEIGHT_NUM; i++)
		free(weights[i]);

  close(serv_sockfd);
  custom_print(114);
	return 0;
}
