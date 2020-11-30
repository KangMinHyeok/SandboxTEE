#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/time.h>

#include <stdlib.h>

#define BUFF_LEN 4096


char dummy[4096<<10];
void run_write(int size, char *filename, int buff_len, int lognum){

  int count = size/buff_len;
  /*
  if (size > buff_len && size % buff_len != 0) {
    printf("size  buff_len mismatch %d %d\n", size, buff_len);
    exit(0);
  }

  if (count == 0) {
    count = 100;
    buff_len = size;
  }
  */
  count = 4;
  buff_len = size;


  int fd = open(filename, O_RDWR|O_CREAT|O_SYNC/*|O_TRUNC*/, 0666);
  //double sec = 1000000000;
  struct timespec tv1, tv2;//, tv3, tv4;
//  clock_gettime(CLOCK_MONOTONIC,&tv1);
  //clock_gettime(CLOCK_MONOTONIC ,&tv3);
  long long elapsed = 0;
  int ret;
  for (int i=0; i< count; i++){
    lseek(fd, 0, SEEK_SET);
  clock_gettime(CLOCK_MONOTONIC ,&tv1);
    ret = write(fd, dummy, buff_len);
  clock_gettime(CLOCK_MONOTONIC ,&tv2);
    if (ret != buff_len)
      printf("error while writing %s [ret: %d, count: %d] \n", filename, ret, size);
  printf("[write_%d] tvsec: %lld\n", size, ((tv2.tv_sec-tv1.tv_sec)*1000000000 + (tv2.tv_nsec-tv1.tv_nsec)));
  elapsed += ((tv2.tv_sec-tv1.tv_sec)*1000000000 + (tv2.tv_nsec-tv1.tv_nsec));
  //printf("[write_%d] tvsec: %lf\n", size, (((double)tv2.tv_sec + (double)tv2.tv_nsec/sec) - ((double)tv1.tv_sec + (double)tv1.tv_nsec/sec ))*sec);
  }
//  struct timeval tv1, tv2;
//  gettimeofday(&tv1, NULL);

  // gettimeofday(&tv2, NULL);
  //clock_gettime(CLOCK_MONOTONIC ,&tv4);

  //printf("[write_%d] tvsec: %lld\n", size*count, ((tv4.tv_sec-tv3.tv_sec)*1000000000 + (tv4.tv_nsec-tv3.tv_nsec)));
  //printf("[write_%d] tvsec: %lld\n", size*count, elapsed);

  close(fd);

}

void run_read(int size, char *filename, int buff_len, int lognum){


  int count = size/buff_len;
  /*
  if (size > buff_len && size % buff_len != 0) {
    printf("size  buff_len mismatch %d %d\n", size, buff_len);
    exit(0);
  }

  if (count == 0) {
    count = 100;
    buff_len = size;
  }
  */
  count = 4;
  buff_len = size;

  //  printf("filename: %s\n", filename);

  int fd = open(filename, O_RDWR|O_CREAT|O_SYNC/*|O_DIRECT*/, 0666);

  struct timespec tv1, tv2;//, tv3, tv4;
  // clock_gettime(CLOCK_MONOTONIC,&tv1);
  //clock_gettime(CLOCK_MONOTONIC ,&tv3);
  //double sec = 1000000000;
  int ret;
  long long elapsed = 0;
  for (int i=0; i< count; i++){
    lseek(fd, 0, SEEK_SET);
  clock_gettime(CLOCK_MONOTONIC ,&tv1);
    ret = read(fd, dummy, buff_len);
  clock_gettime(CLOCK_MONOTONIC ,&tv2);
  printf("[read_%d] tvsec: %lld\n", size, ((tv2.tv_sec-tv1.tv_sec)*1000000000 + (tv2.tv_nsec-tv1.tv_nsec)));
  elapsed += ((tv2.tv_sec-tv1.tv_sec)*1000000000 + (tv2.tv_nsec-tv1.tv_nsec));
  //printf("[read_%d] tvsec: %lf\n", size, (((double)tv2.tv_sec + (double)tv2.tv_nsec/sec) - ((double)tv1.tv_sec + (double)tv1.tv_nsec/sec ))*sec);

    if (ret !=buff_len) {
      printf("error while reading %s [ret: %d, count: %d] \n", filename, ret, buff_len);
      exit(1);
    }
  }

  // clock_gettime(CLOCK_MONOTONIC,&tv2);
  //clock_gettime(CLOCK_MONOTONIC ,&tv4);
  //printf("[read_%d] tvsec: %lld\n", size*count, ((tv4.tv_sec-tv3.tv_sec)*1000000000 + (tv4.tv_nsec-tv3.tv_nsec)));
  //printf("[read_%d] tvsec: %lld\n", size*count, elapsed);

  close(fd);
}


int main(int argc, char *argv[]) {

  //  char total_buff[10*1024*1024];
/*
  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb
  struct rlimit rl;
  int result;

  result = getrlimit(RLIMIT_STACK, &rl);
  if (result == 0)
  {
    if (rl.rlim_cur < kStackSize)
    {
      rl.rlim_cur = kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);
      if (result != 0)
      {
        fprintf(stderr, "setrlimit returned result = %d\n", result);
      }
    }
  }
*/
  struct timespec tv1;
  clock_gettime(CLOCK_MONOTONIC ,&tv1);
  srand (tv1.tv_nsec); 

  int asize = sizeof(dummy)/sizeof(dummy[0]);
  for (int i=0; i<asize; i++){
    dummy[i] = rand()%256;
  }

  int buff_len1[] = {1000, 10000, 100000, 1000000};
  
  //int sizes [] = {800, 1000, 8000, 10000, 80000, 100000};
  int sizes [] = {1000, 2000, 4000, 8000, 16000, 32000, 64000, 128000};
  //int sizes [] = {4096, 4096<<2, 4096<<4};
  for (int i=0; i<1; i++){
    run_write(sizes[0],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[0],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    //run_read (sizes[0],     "/dev/random", buff_len1[i], 11);
    //remove("/home/mkpark/pave/depot_tools/native_client/write_1000");

    run_write(sizes[1],    "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[1],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    //run_read (sizes[1],     "/dev/random", buff_len1[i], 11);
    //remove("/home/mkpark/pave/depot_tools/native_client/write_1000");


    run_write(sizes[2],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[2],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    
    run_write(sizes[3],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[3],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    
    run_write(sizes[4],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[4],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    
    run_write(sizes[5],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[5],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    
    run_write(sizes[6],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[6],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    
    run_write(sizes[7],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (sizes[7],     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    //remove("/home/mkpark/pave/depot_tools/native_client/write_1000");
/*
    run_write(1000000,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    run_read (1000000,   "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 22);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");

    run_write(10000000,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");
    run_read (10000000,   "/home/mkpark/pave/depot_tools/native_client/read_10000000", buff_len1[i], 22);

    run_write(100000000,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");
    run_read (100000000,   "/home/mkpark/pave/depot_tools/native_client/read_10000000", buff_len1[i], 22);

    run_write(1000000000,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len1[i], 11);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");
    run_read (1000000000,   "/home/mkpark/pave/depot_tools/native_client/read_10000000", buff_len1[i], 22);
    */
  }
/*
  int buff_len2[] = {4096, 4096<<2, 4096<<4, 4096<<8};

  for (int i=0; i<1; i++){
    run_write(4096,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 11);
    run_read (4096,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 21);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");

    run_write(4096<<2,  "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 11);
    run_read (4096<<2,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 21);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");

    run_write(4096<<4,  "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 13);
    run_read (4096<<4,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 21);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");

    run_write(4096<<6,  "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 11);
    run_read (4096<<6,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 21);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");

    run_write(4096<<8,  "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 12);
    run_read (4096<<8,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 21);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");

    run_write(4096<<10, "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 12);
    run_read (4096<<10,     "/home/mkpark/pave/depot_tools/native_client/write_1000", buff_len2[i], 21);
    remove("/home/mkpark/pave/depot_tools/native_client/write_1000");
  }
*/
  printf("done!\n");



}
