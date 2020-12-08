#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ifc.h>

#include "util.h"
//#define BUFF_LEN 2048 

//char buffer[BUFF_LEN];
//char profile[BUFF_LEN];


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

int main(int argc, char *argv[]) {

  custom_print(109);
	int BUFF_LEN = 4096;
	char buffer[BUFF_LEN];
	char profile[BUFF_LEN];

	int dsp_num = 2;
	int dsp_port = 7000;

	int ssp_serv_sockfd, ssp_client_sockfd, dsp_sockfd;
  ADTFD adt1;
	struct sockaddr_in ssp_serv_addr;
	int profile_len, len, i, winner;
	double *bid_list;
	char **ad_id_list;
	//double highest_bid;
	int fd_list[dsp_num];
  int ret = 0;	
	
	memset(&ssp_serv_addr, '0', sizeof(ssp_serv_addr));
	ssp_serv_addr.sin_family = AF_INET;
	ssp_serv_addr.sin_addr.s_addr = 0;
	ssp_serv_addr.sin_port = 6000;

	ssp_serv_sockfd = ifc_socket();
	ret = ifc_bind(ssp_serv_sockfd, (struct sockaddr *)&ssp_serv_addr, sizeof(ssp_serv_addr));
  if (ret < 0) {
    printf("bind error\n");
    exit(3);
  }

  adt1 = create_adt(ADT_SIZE);
  custom_print(77);
	ssp_client_sockfd = ifc_accept(ssp_serv_sockfd, adt1);
  if (ssp_client_sockfd < 0){
    close(ssp_serv_sockfd);
    exit(2);
  }

	// profile_len = read_data(ssp_client_sockfd, profile);
	profile_len = unpack_recv_data(adt1, profile);
	// printf("Profile: %s\n", profile);
	

	// printf("SSP connected!!\n");

	memset(buffer, '0', sizeof(buffer));

  ADTFD adt[2];

	for (i=0; i < dsp_num; i++) {
    adt[i] = pack_send_data(profile, profile_len);
		dsp_sockfd = connect_remote(dsp_port + i, adt[i]);
		fd_list[i] = dsp_sockfd; 
		// send_data(dsp_sockfd, profile, profile_len);
	}
	// printf("DSPs connected!!\n");

	bid_list = (double *)malloc(sizeof(double)*(dsp_num));
	ad_id_list = (char **)malloc(dsp_num * sizeof(char *));

	for (i = 0; i < dsp_num; i++) {
		dsp_sockfd = fd_list[i];

		read_data(dsp_sockfd, buffer);

		memcpy(&bid_list[i], buffer, sizeof(bid_list[i]));
		memcpy(&len, buffer+sizeof(bid_list[i]), sizeof(len));
		ad_id_list[i] = (char *)malloc(len * sizeof(char));
		memcpy(ad_id_list[i], buffer+sizeof(bid_list[i])+sizeof(len), len);

		// printf("Id: %d, bid: %.10f, ad id: %s\n", i, bid_list[i], ad_id_list[i]);
	}


	winner = cmp_bids(bid_list, dsp_num);
	// printf("Winner: %d, bid: %.9f, ad id: %s\n\n", winner, bid_list[winner], ad_id_list[winner]);

	memcpy(buffer, &bid_list[winner], sizeof(bid_list[winner]));
	len = strlen(ad_id_list[winner]) + 1;
	memcpy(buffer+sizeof(bid_list[winner]), &len, sizeof(len));
	memcpy(buffer+sizeof(bid_list[winner])+sizeof(len), ad_id_list[winner], len);
 
	send_data(ssp_client_sockfd, buffer, sizeof(bid_list[winner]) + sizeof(len) + len);
 
  custom_print(200);

  for (i = 0; i < dsp_num; i++){
  	send_data(fd_list[i], buffer, sizeof(bid_list[winner]) + sizeof(len) + len);
  }

	for (i = 0; i < dsp_num; i++)
		free(ad_id_list[i]);
	free(ad_id_list);
	free(bid_list);

  close(ssp_serv_sockfd);
  custom_print(110);
	return 0;
}
