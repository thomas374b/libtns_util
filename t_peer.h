/* 
 * Thomas Pantzer: t_peer.h, v0.1b  24.11.98 10:27 
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


#ifndef T_PEER_H
#define T_PEER_H


#ifndef BLOCKED_TIMEOUT
#define BLOCKED_TIMEOUT	12		// measured in seconds
#endif

#include <time.h>
#include <netinet/in.h>


#ifdef AIX
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>

  extern "C" {
	extern void bzero (char *String, int Length);
	}
#endif


// #include "t_ringbuf.h"


class t_reserved_dummy {
public:
	virtual void Done(void) { return; };
};


#define error_from_overloaded_func	0
#define	broadcast_sent_false_func	1
#define process_output_false_func	2
#define process_input_false_func	3
#define process_output_sent_still_false	4




class t_peer {
public:
	int fd,id,soff,bufsize/*,RP,WP*/;
	char connected,sent,broken,sleeping;
	unsigned long address;
	char name[256],nick[256];
	time_t conn_time,timeout;
	
	t_peer *next;

//	char *out_buffer,*in_queue;
//	t_ringbuf  *queue_in, *queue_out;

	char *buffer;
	t_reserved_dummy *reserved;
	
	void Init(int new_fd,sockaddr_in *CA, int new_id);
	t_peer *Insert(int new_fd,sockaddr_in *CA, int new_id);
	int Write(char *st, int l);
	void intError(time_t etime, int func);	
	void Error(time_t etime) { intError(etime, error_from_overloaded_func); };
	
	void Done(void);
};


class t_Peers {
public:
	fd_set Read_FD_Set;

	void Init(int max, int ss_fd);
	void Process(void);
	void Done(void);

	virtual void Broadcast2All(char *st, unsigned int l);
	virtual void Broadcast(char *st, unsigned int l, t_peer *not_this);

	virtual bool Accept(void);
	virtual bool Validate(sockaddr_in *CA);
	virtual void CheckInput(void);
	virtual bool Input(t_peer *p);
	virtual bool Output(t_peer *p); 
	virtual int Write(t_peer *p, char *st, int len); 
	virtual bool Delete(void);
	virtual void Prepare_Select(void);

	t_peer *Last(void);
	t_peer *First(void);
	t_peer *Peer(int i);
	virtual void ReNumber(void);
	int CountAddr(unsigned long a);
//private:
//	virtual char Broadcast(t_peer *p, char *st, int l);
#ifdef GLIBC5
	t_peer *Insert(int new_fd) { return Insert(new_fd,(sockaddr_in *)NULL); };
#else
	t_peer *Insert(int new_fd) { return Insert(new_fd,NULL); };
#endif

protected:
	int FD_MaxNum,count;
	t_peer *first;

	t_peer *Insert(int new_fd,sockaddr_in *CA);

private:
	int maxp,fd_max,maxh;
	int Server_Socket;

};



#endif
