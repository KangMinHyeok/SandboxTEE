/*
 * Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
 * Use of this source code is governed by a GPLv2 license that can be
 * found in the LICENSE file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#include <ifc.h>

#include "util.h"


int main(int argc, char *argv[]) {

  custom_print(101);
	int BUFF_LEN = 1024;
	int serv_sockfd, client_sockfd, analyzer_sockfd;
	struct sockaddr_in serv_addr;
  ADTFD adt1; //, adt2;
	int len;
	char buffer[BUFF_LEN];
	char buffer2[BUFF_LEN];

  int ret = 0;
	int product_id;

#define LEAF_NUMBER 768 
#define MAX_AD 1024
  int ad_bucket[LEAF_NUMBER][MAX_AD];

  len = 0;
  for (int i=0; i< LEAF_NUMBER; i++){
    for (int j=0; j< MAX_AD; j++){
      ad_bucket[i][j] = len++;
    }
  }
  len = 0;

	memset(&serv_addr, '0', sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = 0;
	serv_addr.sin_port = 5000;

	serv_sockfd = ifc_socket();
  ret = ifc_bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (ret < 0){
    printf("bind error\n");
    exit(3);
  }

  printf("init done\n");


  adt1 = create_adt(ADT_SIZE);

  custom_print(77);
	client_sockfd = ifc_accept(serv_sockfd, adt1);
  if (client_sockfd < 0){
    close(serv_sockfd);
    exit(2);
  }

	memset(buffer, '0', sizeof(buffer));
	memset(buffer2, 0, sizeof(buffer2));

	// receive user data
	len = unpack_recv_data(adt1, buffer);

  ADTFD adt = pack_send_data(buffer, len); 
  analyzer_sockfd = connect_remote  (9001, adt);

  read_data(analyzer_sockfd, buffer2);
	

	// gender, age, location, interests
	// 0or1, 2-9, 10-15, 20-531 = 2*8*6*6
	int gender = 0;
	int age = 2;
	int location = 11;
	int interests = 21;

	char *p = buffer;
	memcpy(&gender, p, sizeof(int)); p += sizeof(int);
	memcpy(&age, p, sizeof(int)); p += sizeof(int);
	memcpy(&location, p, sizeof(int)); p += sizeof(int);
	memcpy(&interests, p, sizeof(int)); p += sizeof(int);


  product_id = rand() % MAX_AD;
  int ad_index = 0;
  
#define interests_classify(c, p) {\
  if (c % 6 == 0) ad_index = p ; \
  else if (c % 6 == 1) ad_index = p+1 ; \
  else if (c % 6 == 2) ad_index = p+2 ; \
  else if (c % 6 == 3) ad_index = p+3 ; \
  else if (c % 6 == 4) ad_index = p+4 ; \
  else ad_index = p+5  ; \
}

#define LOC_STRIDE 6
#define location_classify(b, c, p) { \
  if (b == 10)  \
    {interests_classify(c, p);} \
  else if (b == 11) {interests_classify(c, p+LOC_STRIDE ); }\
  else if (b == 12) {interests_classify(c, p+LOC_STRIDE*2 ); }\
  else if (b == 13) {interests_classify(c, p+LOC_STRIDE*3 ); }\
  else if (b == 14) {interests_classify(c, p+LOC_STRIDE*4 ); }\
  else {interests_classify(c, p+LOC_STRIDE*5 ); }\
}

#define AGE_STRIDE 48
#define age_classify(a, b, c, p) {\
  if (a == 2) {  \
    location_classify(b, c, p); \
  } else if (a == 3) { \
    location_classify (b, c, p+AGE_STRIDE ); \
  } else if (a == 4) {\
    location_classify(b, c, p+AGE_STRIDE*2 ); \
  } else if (a == 5) {\
    location_classify(b, c, p+AGE_STRIDE*3 ); \
  } else if (a == 6) {\
    location_classify(b, c, p+AGE_STRIDE*4 ); \
  } else  {\
    location_classify(b, c, p+AGE_STRIDE*5 ); \
  } \
}

  if (gender == 0){
    if (age < 10){
      age_classify(age, location, interests, 0 );
    }
  }
  else {
      age_classify(age, location, interests, 8*6*6);
  }

  product_id = ad_bucket[ad_index][product_id];

	
	len = sizeof(product_id);
	memset(buffer, 0, len+1);
	memcpy(buffer, &product_id, len);

	send_data(client_sockfd, buffer, len);
  custom_print(200);
  
  send_data(analyzer_sockfd, buffer, len);

  close(analyzer_sockfd);
  close(client_sockfd);

  custom_print(102);
	printf("product_id: %x\n", product_id);
	return 0;
}
