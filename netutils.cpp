/* 
 * Thomas Pantzer: netutils.cc, v0.1b  28.1.98 18:6 
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




#include "tns_util/netutils.h"
#include "tns_util/utils.h"
#include "tns_util/daemonic.h"

//#define SHOW_COMPILER_MOD 
#include "tns_util/copyright.h"


#include <string.h>


char *decIPstring(unsigned long address, char *buffer)
{
	unsigned long int i,j;
	unsigned char k,l,m,n;
	
	i = address / 65536;
	j = address % 65536;
	k = i / 256;
	l = i % 256;
	m = j / 256;
	n = j % 256;

#if (__sparc__  || sun || hpux || AIX)
	sprintf(buffer,"%d.%d.%d.%d",k,l,m,n);
#else
	sprintf(buffer,"%d.%d.%d.%d",n,m,l,k);
#endif    
	return buffer;
}

void hostname_str(struct sockaddr_in *Client,char *result)
{
	char st[256],st2[256];
	struct hostent *remote;
	unsigned long ADDR;
	char *st3 = (char*)&ADDR;

	ADDR = Client->sin_addr.s_addr;
	decIPstring(ADDR,st2);
#ifdef DEBUG_SOCKETS  
	EPRINTF("remote address: %s\n",st2); 
	EPRINTF("remote port: %d\n",Client->sin_port);
#endif  
	remote = gethostbyaddr(st3,4,AF_INET); 
	if (remote == NULL) {
#ifdef DEBUG_SOCKETS   
		EPRINTF("gethostbyaddr(%s): %s\n",st3,strerror(errno));
#endif  
		sprintf(result,"%s:%d",st,Client->sin_port);
	} else {
#ifdef DEBUG_SOCKETS
		fprintf(stderr,"remote name: %s\n",remote->h_name);
#endif      
		sprintf(result,"%s:%d",remote->h_name,Client->sin_port);
	} 
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// zum bestimmen der Portnummer aus /etc/services (falls vorhanden)
//________________________________________________________________
int GetTCPPort(char proto[64], int defport)
{
  struct servent *MyService;

  MyService = getservbyname(proto,"tcp");

  if (MyService != NULL) {
#ifdef DEBUG_SOCKETS
        EPRINTF("using port %d from default service for %s\n",
                  ntohs(MyService->s_port),MyService->s_name);
#endif  
        return ntohs(MyService->s_port);  
       } 
     else 
       return defport; 
}  

int clConnect(char *hname, int port_num, char may_fail)
{
  struct hostent *HostName; 

  struct sockaddr_in ServerAddr;
  int _Sock = -1;
  char ST[256];

  HostName = gethostbyname(hname);
 
  if (HostName == (struct hostent *) 0) {
       EPRINTF("gethostbyname(%s): %s\n",ST,strerror(errno));
       if (may_fail == false)
	  exit(2);
	else
	  return -1;
     }
  
  ServerAddr.sin_port=htons(port_num);
  ServerAddr.sin_family=AF_INET;
  ServerAddr.sin_addr.s_addr=*(unsigned long int *)HostName->h_addr;
  
  _Sock = socket(AF_INET,SOCK_STREAM,0);
  if (_Sock==-1) {
     if (may_fail == false) {
	 EPRINTF("socket(): %s\n",strerror(errno));	
	 exit(3);
	}	
      else
	return -1;			
   }
   
  if (connect(_Sock,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr))==-1) {
     if (may_fail == false) {
	 EPRINTF("connect(): %s\n",strerror(errno)); 	
	 exit(4);
	} else {
	 close(_Sock);
	 return -1;		
	}	
   }

#ifdef DEBUG_SOCKETS
  fprintf(stderr,"assigning socket=%d\n",_Sock);
#endif   
  return _Sock; 
}

int clConnect_to(char *hname, int port_num)
{
	return clConnect(hname,port_num,false);
}


void ShutDown(int sd)
{
   struct linger L;

   L.l_onoff = 1;
   L.l_linger = LINGER_TIMEOUT;    
  
#if (linux || hpux || cygwin || Linux)   
   if (setsockopt(sd,SOL_SOCKET,SO_LINGER,&L,sizeof(struct linger)) == -1) {
#else
  #if (__sparc__  || sun || AIX)
   if (setsockopt(sd,SOL_SOCKET,SO_LINGER,(char *)&L,sizeof(struct linger)) == -1) {
  #else
      #error edit typecast of setsockopt for your system
  #endif   
#endif

		EPRINTF("setsockopt(%d): %s\n",sd,strerror(errno));  
	}

   
	int fflag = fcntl(sd,F_GETFL);

	fflag &= ~(O_NDELAY | O_NONBLOCK);
	if (fcntl(sd,F_SETFL,fflag) < 0) {
		EPRINTF("fcntl(%d,SETFL): %s\n",sd,strerror(errno));
	}

   int rt = shutdown(sd,SHUT_WR);
   if (rt < 0) {
	switch(errno) {
		case ENOTCONN:	break;
		default:	EPRINTF("shutdown(%d,1): %s\n",sd,strerror(errno));	break;
	}
	
   }

	if (FD_Ready(sd)) {
		char *dummy = new char [64*1024];
		read(sd,dummy,(64*1024));	
		delete dummy;
	}		
	
   fcntl(sd,F_SETFL,O_NONBLOCK|O_NDELAY);  // get it faster to shut up
   rt = shutdown(sd,SHUT_RDWR);
   if (rt < 0) {
       switch(errno) {
	  case ENOTCONN:
	    break;

	  default:
	    EPRINTF("shutdown(%d,2): %s\n",sd,strerror(errno)); 	
	    break;
	 }	
      }	

//   usleep(50000);

	if (close(sd) < 0) {
		EPRINTF("close(%d) after shutdown: %s\n",sd,strerror(errno));
	}
}


int svAccept(int Sock)
{
   int peer;
#if (LIBC5 || AIX || cygwin)
	int CNameLen;
#else
   unsigned int CNameLen;
#endif
   struct sockaddr_in ClientAddr;
   char st[256];

   CNameLen = sizeof(sockaddr_in);
   peer = accept(Sock,(struct sockaddr *)&ClientAddr,&CNameLen);
   
   hostname_str(&ClientAddr,&st[0]);
#ifdef DEBUG_SOCKETS   
   fprintf(stderr,"connect from %s\n",st);
#endif
   return peer;
}

int svListen(int portx)
{
   return svListen(portx,true);
}

int svListen(int port, char fail)
{
   int L;
   struct sockaddr_in ServerAddr;

   ServerAddr.sin_family = AF_INET;
   ServerAddr.sin_port = htons(port);
   ServerAddr.sin_addr.s_addr = INADDR_ANY;

   L = socket(AF_INET,SOCK_STREAM,0);
   if ((L == -1) && (fail==true)) {
//#ifdef DEBUG_SOCKETS   
       EPRINTF("socket(): %s\n",strerror(errno));
//#endif  
       exit(2);
      } 

   if ((bind(L,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr))==-1) && (fail ==true)) {
//#ifdef DEBUG_SOCKETS   
       EPRINTF("bind(): %s\n",strerror(errno));
//#endif  
       exit(3); 
      }
  
   if ((listen(L,1) == -1) && (fail == true)) {
//#ifdef DEBUG_SOCKETS   
       EPRINTF("listen(): %s\n",strerror(errno));
//#endif  
       exit(4);
      }

//   fcntl(L,F_SETFL,O_NONBLOCK);
   
   return L;
}



