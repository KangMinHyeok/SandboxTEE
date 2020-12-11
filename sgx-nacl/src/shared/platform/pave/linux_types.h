/* -*- mode:c; c-file-style:"k&r"; c-basic-offset: 4; tab-width:4; indent-tabs-mode:nil; mode:auto-fill; fill-column:78; -*- */
/* vim: set ts=4 sw=4 et tw=78 fo=cqt wm=0: */

#ifndef __S_LINUX_TYPES_H__
#define __S_LINUX_TYPES_H__



/*
#include "native_client/src/shared/platform/pave/include/posix_types.h"
#include "native_client/src/shared/platform/pave/include/stat.h"
#include "native_client/src/shared/platform/pave/include/dirent.h"
*/


/*
// mkpark: checked it
struct timespec {
    long tv_sec;
    long tv_nsec;
};

*/

/*
// #include <sys/socket.h>
#include <linux/time.h>
#define __timespec_defined
#undef __USE_POSIX199309
#include <linux/poll.h>
#include <linux/sched.h>
#include <asm/posix_types.h>
#include <asm/fcntl.h>
// #include "sigset.h"
#include <stdint.h>


#ifndef size_t
typedef __kernel_size_t size_t;
#endif

#include <linux/uio.h>

*/
/*
typedef unsigned short int sa_family_t;

struct sockaddr {
    sa_family_t sa_family;
    char sa_data[128 - sizeof(unsigned short)];
};

#ifndef AF_UNIX
# define AF_UNIX 1
#endif

#ifndef AF_INET
# define AF_INET 2
#endif

#ifndef AF_INET6
# define AF_INET6 10
#endif

#ifndef SOCK_STREAM
# define SOCK_STREAM 1
#endif

#ifndef SOCK_DGRAM
# define SOCK_DGRAM 2
#endif

#ifndef SOCK_NONBLOCK
# define SOCK_NONBLOCK 04000
#endif

#ifndef SOCK_CLOEXEC
# define SOCK_CLOEXEC 02000000
#endif

#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0x4000
#endif

#ifndef SHUT_RD
# define SHUT_RD 0
#endif

#ifndef SHUT_WR
# define SHUT_WR 1
#endif

#ifndef SHUT_RDWR
# define SHUT_RDWR 2
#endif

typedef unsigned int socklen_t;

struct msghdr {
    void *msg_name;
    socklen_t msg_namelen;
    struct iovec *msg_iov;
    size_t msg_iovlen;
    void *msg_control;
    size_t msg_controllen;
    int msg_flags;
};

struct cmsghdr {
    size_t cmsg_len;
    int cmsg_level;
    int cmsg_type;
};

#ifndef SCM_RIGHTS
# define SCM_RIGHTS 1
#endif

#define CMSG_DATA(cmsg) ((unsigned char *) ((struct cmsghdr *) (cmsg) + 1))
#define CMSG_NXTHDR(mhdr, cmsg) __cmsg_nxthdr (mhdr, cmsg)
#define CMSG_FIRSTHDR(mhdr) \
    ((size_t) (mhdr)->msg_controllen >= sizeof (struct cmsghdr)		      \
     ? (struct cmsghdr *) (mhdr)->msg_control : (struct cmsghdr *) 0)
#define CMSG_ALIGN(len) (((len) + sizeof (size_t) - 1) \
        & (size_t) ~(sizeof (size_t) - 1))
#define CMSG_SPACE(len) (CMSG_ALIGN (len) \
        + CMSG_ALIGN (sizeof (struct cmsghdr)))
#define CMSG_LEN(len)   (CMSG_ALIGN (sizeof (struct cmsghdr)) + (len))


struct sockopt {
    int receivebuf, sendbuf;
    int receivetimeout, sendtimeout;
    int linger;
    int reuseaddr:1;
    int tcp_cork:1;
    int tcp_keepalive:1;
    int tcp_nodelay:1;
};



#define O_ACCMODE	00000003
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002
#ifndef O_CREAT
#define O_CREAT		00000100	
#endif
#ifndef O_EXCL
#define O_EXCL		00000200
#endif
#ifndef O_NOCTTY
#define O_NOCTTY	00000400	
#endif
#ifndef O_TRUNC
#define O_TRUNC		00001000	
#endif
#ifndef O_APPEND
#define O_APPEND	00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK	00004000
#endif
#ifndef O_DSYNC
#define O_DSYNC		00010000	
#endif
#ifndef FASYNC
#define FASYNC		00020000
#endif
#ifndef O_DIRECT
#define O_DIRECT	00040000	
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE	00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY	00200000	
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW	00400000	
#endif
#ifndef O_NOATIME
#define O_NOATIME	01000000
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC	02000000
#endif

// * Values for the second argument to access.
   These may be OR'd together. 
#  define R_OK	4		// Test for read permission.  //
#  define W_OK	2		// Test for write permission.  //
#  define X_OK	1		// Test for execute permission.  //
#  define F_OK	0		// Test for existence.  //
#define F_GETFL		3	// get file->f_flags //
#define F_SETFL		4	// set file->f_flags //


#define PROT_READ 0x1   // Page can be read.  //
#define PROT_WRITE  0x2   // Page can be written.  //
#define PROT_EXEC 0x4   // Page can be executed.  //
#define PROT_NONE 0x0   // Page can not be accessed.  //
#define PROT_GROWSDOWN  0x01000000  // Extend change to start of
                                                    growsdown vma (mprotect
                                                    only).  //
#define PROT_GROWSUP  0x02000000  // Extend change to start of
                                                  growsup vma (mprotect only).
                                                  //

// Sharing types (must choose one and only one of these).  //
#define MAP_SHARED  0x01    // Share changes.  //
#define MAP_PRIVATE 0x02    // Changes are private.  //

#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_FAILED  ((void *) -1)


// futex
#define FUTEX_PRIVATE_FLAG  128
#define FUTEX_WAIT      0
#define FUTEX_WAKE      1
*/
#endif

