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


#define BUFF_LEN 10*220000 
#define FEAT_NUM 250
#define USER_NUM 501

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
		printf("CAUTION:::: # of features should be matched w/ dataset\n");
		port = atoi(argv[1]);
	}


	// Initialize Cosine Similarity Vector
	fp = fopen("../data/cosine_similarity.txt", "r");
	if (fp == NULL) {
		printf("Cannot open cosine_similarity.txt\n");
		exit(-1);
	}

	double cosine_sim[FEAT_NUM][FEAT_NUM];
	double product_sim;

	// data format idx sim0 sim1 sim2 sim3
	while (fgets(buffer, BUFF_LEN, fp) != NULL) {
		//printf("%s", buffer);

		line = strtok(buffer, " ");
		i = atoi(line);
		j = 0;
		//printf("user: %d\n", user);
		while ((line = strtok(NULL, " "))!= NULL) {
			product_sim = atoi(line);
			cosine_sim[i][j++] = product_sim;
		}
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

	n = read(client_sockfd, buffer, BUFF_LEN);
	if (n < 0) {
		printf("Read error\n");
		goto exit1;
	}

	memcpy(&len, buffer, sizeof(len));
	memcpy(profile, buffer+sizeof(len), len+1);

	printf("%s\n", profile);
	
	int user_index[100];
	int user_feat_num = 0;
	int user_score[FEAT_NUM] = {0, };
	
	i=0;
	line = strtok(profile, " ");
	printf("%s %d\n", __FUNCTION__, __LINE__);
	while (line != NULL) {
		if (line == NULL) break;
		product_id = atoi(line);
		user_index[i++] = product_id;

		line = strtok(NULL, " ");
	}
	user_feat_num = i;


	printf("# of feature: %d\n", user_feat_num);

	for (i=0; i<FEAT_NUM; i++){
		for (j=0; j<user_feat_num; j++){
			if (user_index[j] == i)
				continue;

			user_score[i] += cosine_sim[user_index[j]][i];
		}
	}
	printf("# of feature: %d\n", user_feat_num);

	// get maximum product_id
	product_id = 0;
	int max_score = user_score[0];

	for (i=1; i<FEAT_NUM; i++){
		if (max_score < user_score[i]){
			product_id = i;
			max_score = user_score[i];
		}
	}

	printf("Max score: %d, Ad ID: %d\n", max_score, product_id);

	len = sizeof(product_id);
	memcpy(buffer, &len, sizeof(len));
	memcpy(buffer + sizeof(len), &product_id, len);
	n = write(client_sockfd, buffer, sizeof(len)+len);
	if (n < 0) 
		printf("Write error\n");

exit1:
	close(client_sockfd);
exit2:
	fclose(fp);
	close(serv_sockfd);

	return 0;
	
}
