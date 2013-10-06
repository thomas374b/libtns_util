/* 
 * Thomas Pantzer: utils.h, v0.1b  28.1.98 18:6 
 * 
 * 
 * Copyright 1998 by Thomas Pantzer
 * 
 * Permission to use, copy, modify, and distribute this software for noncommercial 
 * use and without fee is hereby granted, provided that the above copyright notice 
 * appear in all copies and that both that copyright notice and this permission 
 * notice appear in supporting documentation, and that the name of the Author not be 
 * used in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. The Author makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is" 
 * without expressed or implied warranty. 
 * 
 * 
 * pantec@aix520.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */




#ifndef _UTILS_H
#define _UTILS_H


#include <sys/types.h>
/* #include <sys/socket.h> */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "tns_util/win32_export.h"

#if _WINDOWS | WIN32
#else
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/time.h>
	
	extern int __max(int a, int b);
	extern int __min(int a, int b);
	
	extern int FD_Ready(int fd); 
	int FD_Ready(int fd, int to);
	int FD_Writeable(int fd);
#endif



TNS_UTIL_API bool fexist(char *s);
TNS_UTIL_API long int filelen(char *s);	// return -1 if file doesnt exist
TNS_UTIL_API char test_dir(char *s);		// true if s is a directory
TNS_UTIL_API char strnrcmp(char *s1, char *s2, unsigned int len);

TNS_UTIL_API long power(int base, int exp);
// #define pow2(x)	power(2,x)
#define pow2(x)		(1 << x)

TNS_UTIL_API int log2(int q);

TNS_UTIL_API void print_kM(double d);
TNS_UTIL_API char *sprint_kM(double d, char *buf);
	// print a number in %3.1f format with proper unit prefixes to stdout

TNS_UTIL_API char *strip_slash(char *fullpath);		// basename()

//extern void ErrorMsg(const char st[]);
#define ErrorMsg(x)	perror(x)
//extern void ErrorMsg(const char st[],int n);

#ifdef AIX
	#include <sys/select.h>
#endif

// #ifndef FD_SET
// 
// // AIX comes here
// typedef struct fd_set { int fds_bits[1]; } fd_set;
// 
// #define FD_SETSIZE          (sizeof (fd_set) * 8)
// #define FD_SET(f,s)         ((s)->fds_bits[0] |= (1 << (f)))
// #define FD_CLR(f,s)         ((s)->fds_bits[0] &= ~(1 << (f)))
// #define FD_ISSET(f,s)       ((s)->fds_bits[0] & (1 << (f)))
// #define FD_ZERO(s)          ((s)->fds_bits[0] = 0)
// 
// #endif

#ifndef linux

 #ifdef sun
  /* the probles is probaly gone now
  // I can't find the header file for this, but it exists
  extern "C" {
    extern void usleep(unsigned int);
   }
  */
 #else 

 #ifdef hpux
  extern void usleep(long int i);
 #endif

 #endif

#endif


#endif


