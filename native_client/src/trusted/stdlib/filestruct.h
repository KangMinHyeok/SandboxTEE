#ifndef _FILESTRUCT_H_
#define _FILESTRUCT_H_

#ifndef __FILE_defined
#define __FILE_defined 1
#include "native_client/src/trusted/stdlib/stddef.h"
#include <sys/types.h>
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

struct linux_dirent64 {
	unsigned long  d_ino;    /* 64-bit inode number */
	unsigned long  d_off;    /* 64-bit offset to next structure */
	unsigned short d_reclen; /* Size of this dirent */
	unsigned char  d_type;   /* File type */
	char           d_name[]; /* Filename (null-terminated) */
};

#endif
