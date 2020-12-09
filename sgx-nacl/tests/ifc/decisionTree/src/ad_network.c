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
	char *line;
	int i, j;

	if (argc != 2) {
		printf("Usage: ./server [Port]\n ex) ./server 8888\n");
		exit(-1);
	} else{
		printf("CAUTION:::: # of features should be matched w/ dataset\n");
		port = atoi(argv[1]);
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

	// gender, age, location, interests
	// 0or1, 2-9, 10-15, 20-531
	int gender = 0;
	int age = 2;
	int location = 11;
	int interests = 21;

	char *p = profile;
	memcpy(&gender, p, sizeof(int)); p += sizeof(int);
	memcpy(&age, p, sizeof(int)); p += sizeof(int);
	memcpy(&location, p, sizeof(int)); p += sizeof(int);
	memcpy(&interests, p, sizeof(int)); p += sizeof(int);

	// 1bit + 3bit + 4bit + 8bit
	// ad id : (gender+age+location+interests)
	
	product_id = 0;

	product_id += (gender<<15);
	product_id += (age-2) << 12;
	product_id += (location-10) << 8;
	product_id += interests;

	printf("product_id : %x\n", product_id);
	printf("Ad ID: %d\n", product_id);

	len = sizeof(product_id);
	memcpy(buffer, &len, sizeof(len));
	memcpy(buffer + sizeof(len), &product_id, len);
	n = write(client_sockfd, buffer, sizeof(len)+len);
	if (n < 0) 
		printf("Write error\n");


exit1:
	close(client_sockfd);
exit2:
	close(serv_sockfd);

	return 0;
	
}
