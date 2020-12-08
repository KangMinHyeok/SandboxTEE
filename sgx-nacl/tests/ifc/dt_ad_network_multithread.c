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
#include <signal.h>


#define BUFF_LEN 8192 

#define NUMF 24 // 24 good, 32 bad

// char buffer[BUFF_LEN], profile[BUFF_LEN];

int main(int argc, char *argv[]) {

	char *buffer, *profile;
	int serv_sockfd, client_sockfd;
	struct sockaddr_in serv_addr, client_addr;
	int client_len;
	int port, n, len;

	char bytec = 0, bytes = 0;

	int product_id;
	int i, j;

	// for pooling
	int pipefds[2*NUMF];

	pid_t pid, pidl ;

	int index = 0;
	int turn = 0;
	int ret = 0;
	// --------------

	if (argc != 2) {
		printf("Usage: ./server [Port]\n ex) ./server 8888\n");
		exit(-1);
	} else{
		printf("test..\n"); fflush(stdout);
		port = atoi(argv[1]);
	}

	// init process pool

	// create pipe
	for (int i=0; i< NUMF; i++){
		if (pipe(&pipefds[i*2]) < 0)
			goto exit2; 
	} 

	sleep(1);

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

	fflush(stdout);


	// create N processes w/o a parent 
	for (int i=0; i<NUMF; i++){
		index = i; // process index
		pid = fork();

		if (pid < 0)
			goto exit2;
		else if (pid == 0) // child (index: 0~NUMF-1)
			break;
	}

	int status;
	if (pid > 0){
		printf("I'm parents! %d\n", index); fflush(stdout);
		sleep(10);
		index = 0;

		write(pipefds[1], &bytes, 1); // first proc
			printf("watchout\n");
		/*
			 while (read(pipefds[index*4+2], &bytes, 1)){
			 index = (index+1) % NUMF;

			 write(pipefds[index*4+1], &bytes, 1);
			 } */
		while(1) {
			pid = waitpid( -1, &status, 0 );
			if ( pid < 0 ) {
				perror("waitpid");
				break;
			}
			if ( pid == 0 )
				break;
		}
		goto exit2;
	}

run:

	pidl = fork();

	if (pidl > 0){ // parent
		waitpid(-1, &status, 0);
		goto run;
	}	

	// read from master
	close(pipefds[index*2+1]);
	read(pipefds [index*2], &bytes, 1);

restart:

	if ((client_sockfd = accept(serv_sockfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
		printf("Accept error %d\n", client_sockfd); fflush(stdout);

		close(client_sockfd);
		goto restart;
	}

	buffer = malloc(BUFF_LEN);
	profile= malloc(BUFF_LEN);

	// write to next process
	close(pipefds[index*2]);
	write(pipefds[(index+1)%NUMF*2+1], &index, 1);

	n = read_user_data(client_sockfd, buffer, BUFF_LEN);
	//	n = read(client_sockfd, buffer, BUFF_LEN);
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
	// printf("%d %d %d %d\n", gender, age, location, interests);
	product_id = 0;
	product_id += (gender<<15);
	product_id += (age-2) << 12;
	product_id += (location-10) << 8;
	product_id += interests;

	// printf("product_id : %x\n", product_id);
	// printf("Ad ID: %d\n", product_id);
	// fflush(stdout);

	len = sizeof(product_id);
	memcpy(buffer, &len, sizeof(len));
	memcpy(buffer + sizeof(len), &product_id, len);
	// n = write(client_sockfd, buffer, 2048);
	n = send_response(client_sockfd, buffer, 2048);
	if (n < 0) 
		printf("Write error\n");

	free(buffer);
	free(profile);

exit1:
	close(client_sockfd);
exit2:
	close(serv_sockfd);
	exit(1);
}
