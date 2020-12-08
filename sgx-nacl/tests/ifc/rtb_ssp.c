#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ifc.h>

#include "util.h"


int main(int argc, char *argv[]) {

  custom_print(107);
	int BUFF_LEN = 4096;
	
	char buffer[BUFF_LEN];

	int user_serv_sockfd, user_client_sockfd, adex_sockfd;
  ADTFD adt1, adt2;
	struct sockaddr_in user_serv_addr;
	int len;
  int ret = 0;
	//char *ad_id;
	//double bid;

	memset(&user_serv_addr, '0', sizeof(user_serv_addr));
	user_serv_addr.sin_family = AF_INET;
	user_serv_addr.sin_addr.s_addr = 0;
	user_serv_addr.sin_port = 5000;

	user_serv_sockfd = ifc_socket();
	ret = ifc_bind  (user_serv_sockfd, (struct sockaddr *)&user_serv_addr, sizeof(user_serv_addr));
  if (ret < 0) {
    printf("bind error\n");
    exit(3);
  }

  adt1 = create_adt(ADT_SIZE);

  custom_print(77);
	user_client_sockfd = ifc_accept(user_serv_sockfd, adt1);

  if(user_client_sockfd < 0)
    exit(3);

	memset(buffer, '0', sizeof(buffer));
	// len = read_data(user_client_sockfd, buffer);
  len = unpack_recv_data(adt1, buffer);
    
	// printf("profile: %s\n", buffer);
	
//	adex_sockfd = ifc_socket();
//	ifc_connect(adex_sockfd, (uint32_t)&adex_serv_addr, sizeof(adex_serv_addr));
	// send profile to an ad exchange
	// send_data(adex_sockfd, buffer, len);
	adt2 = pack_send_data(buffer, len);
  adex_sockfd = connect_remote(6000, adt2);

		
  int send_size;

	memset(buffer, 0, sizeof(buffer));
	// read result from the ad exchange
	// send_size = read_data(adex_sockfd, buffer);
  send_size = read_data(adex_sockfd, buffer);

	//memcpy(buffer, &bid, sizeof(bid));
	//memcpy(buffer + sizeof(bid), &len, sizeof(len));
	//memcpy(buffer + sizeof(bid) + sizeof(len), ad_id, len);
	//printf("Bid: %f, Bid id: %s\n", bid, ad_id);
  
	// send result to the user
	send_data(user_client_sockfd, buffer, send_size);
  custom_print(200);

  close(user_serv_sockfd);
  custom_print(108);
	return 0;

}
