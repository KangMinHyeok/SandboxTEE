#ifndef _FILES_H_
#define _FILES_H_

#ifndef __FILE_defined
#define __FILE_defined 1
typedef struct 
{
  short level ;
  short token ;
  short bsize ;
  char fd ;
  unsigned flags ;
  unsigned char hold ;
  unsigned char *buffer ;
  unsigned char * curp ;
  unsigned istemp; 
}FILE ;
#endif
FILE * fopen(const char *file, const char *mode);
int fseeko(FILE *fp, off_t offset, int whence);
int fseek(FILE *fp, long offset, int whence);
size_t fread(void *buf, size_t size, size_t count, FILE *fp);
#ifndef ferror
int ferror(FILE *fp);
#endif
size_t fwrite(const void *buf, size_t size, size_t count, FILE *fp);
long ftell(FILE *fp);
int fflush(FILE *fp);
int fclose(FILE *fp);

#endif
