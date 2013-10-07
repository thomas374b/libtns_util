/* 
 * Thomas Pantzer: netutils.h, v0.1b  28.1.98 18:6 
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




#ifndef _NETUTILS_H
#define _NETUTILS_H

// #ifndef u_long
// #define u_long	unsigned long
// #define u_char	unsigned char
// #define u_short	unsigned short
// #endif

#include "tns_util/porting.h"

#include <sys/types.h>

#ifdef AIX
	extern "C" {

	extern int socket (int AddressFamily, int Type, int Protocol);
//  	extern int connect (int Socket,struct sockaddr * Name,int NameLength);
//  	extern int setsockopt (int Socket, int Level, int OptionName, char *OptionValue, int OptionLength);
	extern int shutdown (int Socket, int How);
  	extern int accept (int Socket,struct sockaddr *Address,int *AddressLength);
//	extern int bind (int Socket, struct sockaddr *Name, int NameLength);
	extern int listen (int Socket, int Backlog);

	}

//	#ifdef cplusplus
//	  #error CPLUSPLUS defined
//	#endif
#endif


#if _WINDOWS | WIN32
#else
	#include <sys/socket.h> 
	#include <netinet/in.h>
	#include <netdb.h>
#endif




#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>



#ifndef LINGER_TIMEOUT
#if (__sparc__ || sun || linux || AIX)
  #define LINGER_TIMEOUT	6	// 6 seconds
#else  
  #define LINGER_TIMEOUT	20    
#endif
#endif


TNS_UTIL_API int GetTCPPort(char proto[64],int defport);
extern void hostname_str(struct sockaddr_in *client,char *result);
extern char *decIPstring(unsigned long address, char *buffer);

extern void ShutDown(int sd);	// extra strong linger shutdown sequence 

TNS_UTIL_API void setNonblockingIO(int sd, bool noBlock);
extern int clConnect(char *hname, int port_num, char may_fail);
extern int clConnect_to(char *, int);

extern int svListen(int);
extern int svListen(int,char);
extern int svAccept(int);
//extern void SendMsg(int, char *);

#endif
