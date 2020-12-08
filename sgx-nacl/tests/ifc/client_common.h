	
#include <time.h>


#define BUFF_LEN 2048
struct timeval before, middle , after;


static int send_data(int sockfd, char *buffer, int len) {
	int n;

  char sendbuf[BUFF_LEN];
  memset(sendbuf, 0, BUFF_LEN);
  
  memcpy(sendbuf, &len, sizeof(len));
  memcpy(sendbuf + sizeof(len), buffer, len);
  
  printf("send %d data\n", len);
	n = write(sockfd, sendbuf, BUFF_LEN);
	return n;
}

static int read_data(int sockfd, char *buffer) {
	int len;
	int n = 0;

  char sendbuf[BUFF_LEN];
  memset(sendbuf, 0, BUFF_LEN);
  
	read(sockfd, sendbuf, BUFF_LEN);
  
  memcpy(&len, sendbuf, sizeof(len));
  printf("read %d data\n", len);
  memcpy(buffer, sendbuf + sizeof(len), len);
  
	return len;
}

static double time_diff(struct timeval x , struct timeval y)
{
	double x_us , y_us , diff;

	x_us = (double)x.tv_sec*1000000 + (double)x.tv_usec;
	y_us = (double)y.tv_sec*1000000 + (double)y.tv_usec;

	diff = (double)y_us - (double)x_us;

	return diff;
}
