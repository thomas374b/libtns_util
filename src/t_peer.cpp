/* 
 * Thomas Pantzer: t_peer.cc, v0.1b  24.11.1998 10:27 
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


#include <string.h>
#include <unistd.h>

#include "tns_util/porting.h"
#include "tns_util/t_peer.h"
#include "tns_util/utils.h"
#include "tns_util/netutils.h"
#include "tns_util/daemonic.h"

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"

const char *internal_errmsg[5] = {
	"error_from_overloaded_func",      
	"broadcast_sent_false_func",       
	"process_output_false_func",      
	"process_input_false_func",       
	"process_output_sent_still_false" 
};



void t_peer::Init(int new_fd,sockaddr_in *CA, int new_id)
{
    time_t TM;
    char st[256],*s;

	fd = new_fd;
	id = new_id;
	connected = true;
	sent = true;	// indicate empty sendqueue
	broken = false;	// indicate cbreak mode	
	silent = false;	// if true, do not participate in Read/Write/Input/Output
	soff = 0;		// special offset (reserved for future use)
	bufsize = 0;
	next = NULL;

//	queue_out = NULL;
//	queue_in = NULL;	

	buffer = NULL;    
	reserved = NULL;

    /*  // reserved for future use	 
    queue = NULL;
    RP = 0;
    WP = 0;	
    */	

	setNonblockingIO(fd, true);

	if (CA != NULL) {
		address = CA->sin_addr.s_addr;
		hostname_str(CA,name);
	} else {
		address = 0;
		strcpy(name,"NULL\0");	
	}

    strcpy(nick,"\0");
 	
    conn_time = time(&TM);
   
    sprintf(st,"%s",ctime(&TM));
    s = strchr(st,'\n');

    if (s != NULL)
       *s = '\0';

#ifdef VERBOSE 
    fprintf(stderr,"connect \aa:t %s from: %s\n",st,name);
#endif	     
}

int t_peer::Read(int len)
{
	if (silent) {
#ifdef DEBUG
		LPRINTF("thread: %d\tRead(%d) filedescriptor [%d] silenced", getpid(), len, fd);
#endif
		memset(buffer, 0, len);
		return 0;
	}
	return readSock(fd, buffer, len);
}


int t_peer::Write(int fd, char *s, int len)
{
	if (silent) {
#ifdef DEBUG
		LPRINTF("thread: %d\tWrite(%d,*,%d) filedescriptor silenced", getpid(), fd, len);
#endif
		return len;
	}
	if (FD_Writeable(fd)) {
		int ret = writeSock(fd, s, len);
/*
		if (fsync(fd) == -1) {
			EPRINTF("can't fsync fd [%d]: %s", fd, strerror(errno));
		}
*/
		return ret;
	} else {
		EPRINTF("filedescriptor [%d] not writeable: %s", fd, strerror(errno) );
	}
	return 0;
}

int t_peer::Write(char *buf, int len)
{	
	int wr = 0;
        
	if (broken == true) {
	    int w = 0;
   	    char s[4] =  {0x0d,0x0a,0,0};
 	
    	for (int i=0; i<len; i++) {
			if (buf[i] == '\n')
				w += Write(fd,s,2);
			else
				w += Write(fd,&buf[i],1);

  	    	wr++;	
		}
	     // we ignore the number of written bytes from those
	     // shitty NT-telnet clients
	} else {
		wr = Write(fd, buf, len);
// 	    *buf = '\0';
	}
	if ((len > wr) && (wr > 0)) {
		soff = wr;
	    sent = false;
	} else {
		soff = 0;
		sent = true;     
	}	

//	fflush(fd);
// 	if (wr == len)
// 	   sent = true;
// 	 else
// 	   sent = false;     
	   
	return wr;   
}


t_peer *t_peer::Insert(int new_fd,sockaddr_in *CA, int new_id)
{
    if (next == NULL)  {
        next = new t_peer;
		next->Init(new_fd,CA,new_id);
		return next;
	} 
    return next->Insert(new_fd,CA,new_id);  
}


void t_peer::intError(time_t etime, int func)
{
//	char st[256];
      switch(errno)  {
         case 0:
         case 2:
//         case 9:    // bad file number
			case EAFNOSUPPORT:	
			case EBADF:
			case EFAULT:
//         case 14:   // bad address
			case EPIPE:	//  case 32:   // brocken pipe
			case ENOTCONN:
			case ECONNREFUSED:
			case ECONNRESET:
#ifdef DEBUG
				EPRINTF("t_peer::Error(): %s from %s\n",strerror(errno),internal_errmsg[func]);
#endif
				connected = false;
				break;
	   
			case EINTR:	//	 case 4:	// interrupted system call (sun)	
#ifdef DEBUG
				EPRINTF("t_peer::Error(): %s from %s\n",strerror(errno),internal_errmsg[func]);	
#endif	   	
				if (func == process_input_false_func) {
					EPRINTF("input must be greater than 0 if select was true, assume disconnect");
					connected = false;
				}
	// programs, who want disconnect here	1
	// programs, who want ignore this	?
				break;  
                 
		    
			default:
				EPRINTF1("default handled error %d: %s\n", errno, strerror(errno));
//         case 11:
			case EWOULDBLOCK:
				if ((etime - timeout) > BLOCKED_TIMEOUT) {
#ifdef DEBUG
					EPRINTF("EWOULDBLOCK timeout\n");
#endif	       
					connected = false;
				} 
				break;    
        } // switch(errno)
}


void t_peer::Done(void)
{
#ifdef DEBUG
	EPRINTF("closing socket %s\n",name);
#endif
	ShutDown(fd);

// 	if (queue_out != NULL) {
// 	    queue_out->Done();	
//             delete queue_out;	
// 	    queue_out = NULL;	
// 	   }
//  	
// 	if (queue_in != NULL) {
// 	    queue_in->Done();	 
//             delete queue_in;	
// 	    queue_in = NULL;
// 	   } 

	if (reserved != NULL) {
		reserved->Done();
		delete reserved;
		reserved = NULL;
	}

	if (buffer != NULL) {
		delete buffer;
		buffer = NULL;
	}
}


t_peer *t_Peers::Insert(int new_fd, sockaddr_in *CA)
{
	if (first == NULL) {
		first = new t_peer;
		first->Init(new_fd, CA, count);
		return first;
	} else {
	   return first->Insert(new_fd, CA, count);
	}
}



void t_Peers::Init(int m, int ss_fd, bool th)
{
	maxp = m * m;
	maxh = m;
	Server_Socket = ss_fd;
	child = -1;
	threaded_mode = th;

	setNonblockingIO(Server_Socket, true);

	count = 0;
	first = NULL;
	insertedLast = NULL;
}




t_peer *t_Peers::Last(void)
{
	t_peer *p = first;
	
	if (p == NULL)
	   return p;
	
	while (p->next != NULL)
	  p = p->next;

	return p;
}

void t_Peers::ReNumber(void)
{
	t_peer *p = first;
	
	if (p == NULL)
	   return;
	
	int cid = 0;
	while (p != NULL)
	  {
	   p->id = cid;
	   cid++;		
	   p = p->next;
	  }	
#ifdef DEBUG
	EPRINTF("t_Peers::ReNumber(%d)\n",cid);
#endif
}

t_peer *t_Peers::Peer(int id)
{
	if (id < 0)
	   return NULL; 	

	t_peer *p = first;
	
	if (p == NULL)
	   return p;
	
	while ((p != NULL) && (p->id != id))
	  p = p->next;

	return p;
}

t_peer *t_Peers::First(void)
{
	return first;
}

int t_Peers::CountAddr(unsigned long a)
{
	t_peer *p = first;
	
	if (p == NULL)
	   return 0;
	
	int cnt = 0;
	while (p != NULL)
          {
	   if (p->address == a)
	      cnt++;
		
	   p = p->next;
	  } 

	return cnt;
}


bool t_Peers::Validate(sockaddr_in *CA)
{
	int k = CountAddr(CA->sin_addr.s_addr);

	if (k > maxh)
	   return false;

	return true;
}

bool t_Peers::Accept()
{
	if (threaded_mode) {
		if (Server_Socket == -1) {
			return false;	// ignore closed server socket and continue
		}
	}

	if (!FD_ISSET(Server_Socket, &Read_FD_Set))
		return false;

//    if (FD_Ready(Server_Socket) == false)
//       return false;

    if (count >= maxp)
       return false;	

    struct sockaddr_in ClientAddr;
#if (LIBC5 || AIX || cygwin || WIN32)
    int CNL = sizeof(ClientAddr);
#else
    unsigned int CNL = sizeof(ClientAddr);
#endif
    int new_fd = accept(Server_Socket,(struct sockaddr *)&ClientAddr,&CNL);

	if (new_fd == -1) {
		EPRINTF("accept(): %s\n",strerror(errno));
		return false;
	}

    if (threaded_mode) {
//    	if (child == -1) {
    		child = fork();
//    	}

    	switch(child) {
    		case -1:	// error code, could not fork
        		EPRINTF("cant fork child: %s", strerror(errno));
        		return false;

    		case 0:		// I'm the child
    			if (Server_Socket != -1) {
    				Server_Socket = -1;
    			}
    			break;	// continue this function

    		default:	// I'm parent
    			close(new_fd);
    			return false;
    	}
    }

	if (Validate(&ClientAddr) == false) {
		ShutDown(new_fd);
		return false;
	} 
	insertedLast = Insert(new_fd, &ClientAddr);
 
//        fd_max = max(fd_max,new_fd);
	count++;
	ReNumber();
#ifdef DEBUG_SOCKETS	
	EPRINTF("t_Peers::Accept(%d)\n",count);
#endif
	return true;	
}

bool t_Peers::Input(t_peer *p)
{
#ifdef DEBUG_SOCKETS
	fprintf(stderr,"t_Peers::Input(basic): virtual method not overloaded\n");
#endif	
	if (p->fd <0)
	   return false;

	return true;
}

bool t_Peers::Output(t_peer *p)
{
#ifdef DEBUG_SOCKETS
	fprintf(stderr,"t_Peers::Output(0x%lx): virtual method not overloaded\n",(long)p);
#endif
	if (p->fd <0)
	   return false;

	return true;
}


int t_Peers::Write(t_peer *p, char *st, int l)
{
	return p->Write(st,l);
}


void t_Peers::Broadcast(char *st, unsigned int l, t_peer *N)
{
	t_peer *peer = first;

	while (peer != NULL) {
		if (N == peer) {			// if not NULL don't sent to this peer
			peer = peer->next;
			continue;
		}
	      	
		if (peer->connected == true) {
#ifdef DEBUG
			fprintf(stderr,"t_Peers::Broadcasts(0x%lx)\n",(long)peer);
#endif	       
			if (peer->reserved != NULL) {
				if (peer->reserved->blockedBroadcast(N)) {
					// this peer was elected _not_ to receive broadcasts from N
#ifdef DEBUG
					fprintf(stderr, "skip broadcasting for peer %08lx", peer->address);
#endif
					peer = peer->next;
					continue;
				}
			}

			int L = l;
			int w = Write(peer,st,L);
			while (w < (int)l)	{
				L -= w;
				int k = Write(peer,&st[w],L);
				if (k > 0) {
					w += k;
				} else {
					break;
				}
			}

			if (peer->sent == false) {
				peer->intError(time(0), broadcast_sent_false_func);
			}
		}
		peer = peer->next;
	}
}

void t_Peers::Broadcast2All(char *st, unsigned int l)
{
	Broadcast(st,l,NULL);
}

void t_Peers::Prepare_Select(void)
{
// alle aktiven File-Descriptoren sollen von "select()" gleichzeitig 
// geprüft werden
	FD_ZERO(&Read_FD_Set);	

	FD_MaxNum = 0;
	FD_MaxNum = __max(Server_Socket, FD_MaxNum);
	if (Server_Socket != -1) {
		FD_SET(Server_Socket, &Read_FD_Set);
	}

	t_peer *peer = first;
	while (peer != NULL) {
		if ((peer->connected == true) && (peer->silent == false)) {
			FD_SET(peer->fd, &Read_FD_Set);

			if (FD_MaxNum < peer->fd)
				FD_MaxNum = peer->fd; 
		}
		peer = peer->next;
	}
}


// unchain only without deletion of pointers, remove from select-set
void t_Peers::Ignore(t_peer *peer)
{
	t_peer *p;

	if (first == NULL) {
	   return;
	}
	p = first;
	while (p != NULL) {
		if (p == peer) {
			p->silent = true;
			// remove fd from select set
			// ignore in processing Input/Output/Write/Read
			close(p->fd);
		}
		p = p->next;
	}
}

bool t_Peers::Delete(void)
{
//	fprintf(stderr,"t_Peers::Delete()\n");
	t_peer *peer,*delp;

	if (first == NULL)
	   return false;
	   
	int t = count;

	while (first->connected == false) {
	   first->Done();
	   delp = first;
	   
	   first = first->next;	
#ifdef DEBUG_SOCKETS	   
	   EPRINTF("t_Peers::Delete(0x%lx)\n",(long)delp);
#endif	   
	   delete delp;
	   count--;
	   
	   if (first == NULL) {
	       if (t != count)
		   return true;
		else
	           return false;
	      }
	  } 
	
	peer = first;
	while (peer->next != NULL) {
	   if (peer->next->connected == false) {
	       delp = peer->next;
	       peer->next = peer->next->next;
	       delp->Done();
#ifdef DEBUG_SOCKETS
  	       EPRINTF("t_Peers::Delete(0x%lx)\n",(long)delp);
#endif	       
	       delete delp;
	       count--;
	      }
	    else
	      peer = peer->next; 
	  }  


	if (t != count) {
	    ReNumber();
	    return true;
	}
	return false;
}



void t_Peers::CheckInput()
{
	Prepare_Select();

	struct timeval TimeOut;
	TimeOut.tv_sec=0;
	TimeOut.tv_usec=10000;

	select(FD_MaxNum+1,&Read_FD_Set,0,0,&TimeOut);

	Accept();
}


void t_Peers::Process()
{
	CheckInput();

	t_peer *peer = first;

	while (peer != NULL) {
		if (FD_ISSET(peer->fd,&Read_FD_Set)) {
			if (peer->connected == true) {
#ifdef DEBUG_SOCKETS
				EPRINTF("t_Peers::Process(0x%lx)\n",(long)peer);
#endif	       
			}
			if (Input(peer) == true) {
				// reset timeout counter because some bytes were read
				peer->timeout = time(0);
				if (Output(peer) == false) {
					peer->intError(time(0), process_output_false_func);
				} else {
					// reset timeout counter because some bytes were written
					peer->timeout = time(0);	
				}	
			} else {
				peer->intError(time(0), process_input_false_func);
			}	
		} else {
// 	       long to = time(0) - peer->timeout;
// 	       if (to > MAX_TIMEOUT)
// 		  {	// hart rausschmeißen
// 		   peer->connected == false;
// 		   Delete();		
// 		  }  	
		}	
		if (peer->sent == false) {
			if (Output(peer) == false) {
				peer->intError(time(0),process_output_sent_still_false);  
			} else {
				// reset timeout counter because some bytes were written
				peer->timeout = time(0);
			}	
		}	
		peer = peer->next;   
	}   
//	usleep(100000);
	Delete();
}



void t_Peers::Done(void)
{
    t_peer *delp;

    if (first == NULL)
       return;

    while (first != NULL) {
       first->Done();
       delp = first;

       first = first->next;	
       delete delp;
       count--;
      } 
}





/* END-OF-FILE */




