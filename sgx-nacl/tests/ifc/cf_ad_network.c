#define FLT_RADIX 2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ifc.h>

#include "util.h"

int main(int argc, char *argv[]) {

  custom_print(103);
	int BUFF_LEN = 8192;
	int FEAT_NUM = 250;
  int ret = 0;

	int serv_sockfd, client_sockfd, analyzer_sockfd;
	struct sockaddr_in serv_addr;
  ADTFD adt1; //, adt2;

	double cosine_sim[FEAT_NUM][FEAT_NUM];
	double product_sim;

	char buffer[BUFF_LEN];
	char buffer2[BUFF_LEN];

	int user_index[100];
	int user_feat_num = 0;
	double max_score;
	double user_score[FEAT_NUM];
	
	int product_id;
	FILE *fp = NULL;
	char *line;
	int i, j;
	int len, count;

	memset(buffer, '0', sizeof(buffer));
	memset(buffer2, 0, sizeof(buffer2));


	// Initialize Cosine Similarity Vector
	fp = fopen("/home/hmlee/pave/depot_tools/native_client/tests/ifc/confil/data/cosine_similarity.txt", "r");
	if (fp == NULL) {
		printf("Cannot open cosine_similarity.txt\n");
		return 0;
	}

	i = 0;
	// data format sim0 sim1 sim2 sim3
	while (fgets(buffer, BUFF_LEN, fp) != NULL) {
		//printf("profile: %s\n", buffer);
		j = 0;

		line = strtok(buffer, " ");
		while (line != NULL) {
			product_sim = atof(line);
			cosine_sim[i][j++] = product_sim;
			line = strtok(NULL, " ");
			if (strcmp(line, "\n") == 0)
				break;
		}
		i++;
	}

	fclose(fp);

	// read user profile
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = 0;
	serv_addr.sin_port = 5000;

	serv_sockfd = ifc_socket();

	ret = ifc_bind(serv_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (ret < 0)
    exit(3);

  adt1 = create_adt(ADT_SIZE);

  custom_print(77);
	client_sockfd = ifc_accept(serv_sockfd, adt1);
  if (client_sockfd < 0){
    close(serv_sockfd);
    exit(2);
  }


	// read_data(client_sockfd, buffer);
  // get_adt_entry(adt1, buffer, 1024);
	len = unpack_recv_data(adt1, buffer);

  ADTFD adt = pack_send_data(buffer, len); 
  analyzer_sockfd = connect_remote  (9001, adt);
  // send_data(analyzer_sockfd, buffer, len);  

  read_data(analyzer_sockfd, buffer2);
	
  char *p = buffer;
  p += sizeof(int);

	for (i=0; i<FEAT_NUM; i++)
		user_score[i] = 0;
	
	i=0;
	line = strtok(p, " ");
	while (line != NULL) {
		if (line == NULL) break;
		product_id = atoi(line);
		user_index[i++] = product_id;

		line = strtok(NULL, " ");
	}
	user_feat_num = i;

	count = 0;
	for (i=0; i<FEAT_NUM; i++){
		for (j=0; j<user_feat_num; j++){
			if (user_index[j] == i)
				continue;

			count++;
			user_score[i] += cosine_sim[user_index[j]][i];
		}
	}

	// get maximum product_id
	product_id = 0;
	max_score = user_score[0];

	for (i=1; i<FEAT_NUM; i++){
		if (max_score < user_score[i]){
			product_id = i;
			max_score = user_score[i];
		}
	}

	// printf("Max score: %f, Ad ID: %d\n", max_score, product_id);

	len = sizeof(product_id);
	memcpy(buffer, &product_id, len);

	send_data(client_sockfd, buffer, len);
  custom_print(200);

  // printf("after send %d\n", len);
  
  send_data(analyzer_sockfd, buffer, len);

  close(client_sockfd);
  close(analyzer_sockfd);
  close(serv_sockfd);

  custom_print(104);
	return 0;
}
