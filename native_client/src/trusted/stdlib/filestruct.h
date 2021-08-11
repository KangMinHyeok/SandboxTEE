#ifndef _FILESTRUCT_H_
#define _FILESTRUCT_H_

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
  int _fileno; 
}FILE ;

#endif

#endif
