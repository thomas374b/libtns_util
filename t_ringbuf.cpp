/* 
 * Thomas Pantzer: t_ringbuf.cc, v0.1b  6.8.99 16:10 
 * 
 * 
 * Copyright 1999 by Thomas Pantzer
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
 * pantec@zorro.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */


#include <stdio.h>
#include <string.h>


#include "tns_util/porting.h"
#include "tns_util/t_ringbuf.h"
#include "tns_util/utils.h"
#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"


void t_ring_buf::Init(int len, int p, int g)
{
	get_size = g;
	put_size = p;
	
//	size = ((__max(len,__max(p+1,g+1)*2) / get_size) + 1) * get_size;
	size = __max(len,__max(p+1,g+1)*2);
	size += 1024;
	
//	real_size = size + __max(p+1,g+1) + 1024;
	real_size = (size*2) + 1024;

	fprintf(stderr,"l:%d, put:%d, get:%d, size %d, real:%d\n",len,p,g,size,real_size);
	
	Array = new char [real_size];

	Reset();	
}

void t_ring_buf::Done(void)
{
	delete Array;
	Array = NULL;
}

int t_ring_buf::Put_Avail(void)
{
	int a = wptr - rptr;
	if (a <= 0)
	   a += size;
	   
	return (a-3);	   
}

bool t_ring_buf::bGetAvail(void)
{
	return (rptr != wptr);
}

char t_ring_buf::GetSingle(void)
{
	char *ret = wptr;
	GetIncrement(1);	
	return *ret;
}

int t_ring_buf::Get_Avail(void)
{
	int a = rptr - wptr;
	if (a < 0)
	   a += size;
	
	return (a-3);	   
}

void t_ring_buf::GetWrap(void)
{
	if (tmp_wptr >= &Array[size]) {
		hangover_ptr = tmp_wptr;
		
	    if (tmp_wptr > &Array[size]) {
			hangover = tmp_wptr - &Array[size];

			memmove(&Array[size],&Array[0],hangover);	   

			if (hangover >= size) {
				fprintf(stderr,"implementation flaw, hangover exceed array size by %d\n",hangover - size);
				hangover = size -1;
			}
		    tmp_wptr = &Array[hangover];
	    } else {
			hangover = 0;
		    tmp_wptr = &Array[0];
		}	
	} else {
		hangover_ptr = 0;
	}	   
}


char *t_ring_buf::GetBlockAddress(int *free)
{
#ifdef DEBUG_ALL
	fprintf(stderr,"t_ring_buf::GetAddress(0x%08lx)\n",(long)free);
#endif
	if (tmp_wptr != NULL) {
//		GetIncrement(get_size); 
		GetIncrement(); 	 
			// assume getting has been finished before second or 
			// further calls to GetAddress()
	}

	*free = Get_Avail();
	if (*free < get_size) {
		return NULL;
	}
	
	tmp_wptr = wptr + get_size;
	
	GetWrap();
	
	return (char *)wptr;
}

char *t_ring_buf::GetAddress(int *free, int request)
{
#ifdef DEBUG_ALL
	fprintf(stderr,"t_ring_buf::GetAddress(0x%08lx)\n",(long)free);
#endif
	if (tmp_wptr != NULL) {
		GetIncrement();  
			// assume getting has been finished before second or 
			// further calls to GetAddress()
	}

	*free = Get_Avail();
	if (*free <= 0) {
		return NULL;
	}
	int get = __min(*free,request);
	
	tmp_wptr = wptr + get;
	
	GetWrap();

	return (char *)wptr;
}


void t_ring_buf::GetIncrement(void)
{
#ifdef DEBUG_ALL
	fprintf(stderr,"t_ring_buf::GetIncrement()\n");
#endif
	if (tmp_wptr != NULL) {
		wptr = tmp_wptr;

		tmp_wptr = NULL;
	} else {
		fprintf(stderr,"implementation flaw, GetIncrement() should be called only once after GetAddress()\n");
	}
}

void t_ring_buf::GetIncrement(int step)
{
	char *new_ptr = wptr + step;
	
	if ((new_ptr > hangover_ptr) && (new_ptr > &Array[size])) {
		int draw_noise = new_ptr - &Array[size];
		fprintf(stderr,"implementation flaw, possibly got %d bytes noise from behind buffer\n",draw_noise);
		fprintf(stderr,"new_ptr: %ld, hangover_ptr: %ld, tmp_wptr:%ld, wptr:%ld, step:%d, hangover:%d\n",
				(long)new_ptr,(long)hangover_ptr,(long)tmp_wptr,(long)wptr,step,hangover);
				
		exit(0);
	} else {
		wptr = new_ptr;
		if (wptr >= &Array[size]) {
		    if (wptr > &Array[size]) {
				int hangover = wptr - &Array[size];

				if (hangover >= size) {
					fprintf(stderr,"implementation flaw, hangover exceed array size by %d\n",hangover - size);
					hangover = size -1;
				}
			    wptr = &Array[hangover];
		    } else {
			    wptr = &Array[0];
			}	
		}	
	}
	tmp_wptr = NULL;	   
}

/*
void t_ring_buf::PutBack(int b)
{
	rptr -= b;
	
	if (rptr < &Array[0])
		rptr += size;
}

void t_ring_buf::GetBack(int b)
{
	char *oldp = wptr - b;
	
	if (oldp < &Array[0]) 
		oldp += size;
		
	memmove(wptr,oldp,b);	 
}

void t_ring_buf::Put(char *buf,int s)
{
	memcpy(rptr,buf,s);
	rptr += s;
	
	PutWrap();
}
*/

int t_ring_buf::Put(char *buf,int s)
{
#ifdef DEBUG_ALL
	fprintf(stderr,"t_ring_buf::Put(0x%08lx,%d)\n",(long)buf,s);
#endif	
	int avail = 0;
	char *b = PutAddress(&avail);
	if ((b == NULL) || (avail < 1)) {
		return 0;
	}
	// rptr is equal to b
	memcpy(rptr,buf,avail);

	PutIncrement(avail);

	return avail;
}


void t_ring_buf::PutWrap(void)
{
#ifdef DEBUG_ALL
	fprintf(stderr,"t_ring_buf::PutWrap\n");
#endif
	if (rptr >= &Array[size]) {
		if (rptr > &Array[size]) {
			long wrap = rptr - &Array[size];
			memmove(&Array[0],&Array[size],wrap);
	    
			if (wrap >= size) {
				fprintf(stderr,"implementation flaw, wrap exceed array size by %ld\n",wrap - size);
				wrap = size -1;
			}
			rptr = &Array[wrap];
		} else {
			rptr = &Array[0];
		}	
	}	
}

void t_ring_buf::PutIncrement(int inc)
{
#ifdef DEBUG_ALL
	fprintf(stderr,"t_ring_buf::PutIncrement(%d)\n",inc);
#endif
	if ((rptr + inc) > &Array[real_size]) {
		fprintf(stderr,"implementation flaw, PutIncrement higher than allocated buffer\n");
		rptr = &Array[real_size -2];
	} else {
		rptr += inc;
	}	
	PutWrap();
}


char *t_ring_buf::PutAddress(int *avail, int threshold)
{
#ifdef DEBUG_ALL
	fprintf(stderr,"t_ring_buf::PutAddress(0x%08lx)\n",(long)avail);
#endif
	*avail = Put_Avail();
	if (*avail < threshold) {
		return NULL;
	}
	return (char *)rptr;
}

/*
char *t_ring_buf::Put(int inc)
{
	if (inc == 0) {
		fprintf(stderr,"obsoleted function called, use PutAddress() instead\n");
		int space;
		return PutAddress(&space);
	}
	
	int toPut = inc;
	int avail = Put_Avail();

	if (toPut > avail) {
		toPut = avail -1;
	}
	if (toPut <= 0) {
		return NULL;
	}
	char *buf = rptr;
	rptr += toPut;

//	PutWrap();
	return buf;	   
}

*/

int t_ring_buf::ReadFrom(fileHandle fd)
{
	int canRead = 0;
	char *Buf = PutAddress(&canRead);	

	if ((Buf != NULL) && (canRead > 0)) {
//		fprintf(stderr,"gsmBuf: 0x%08lx, canGet:%d, fd:%d\n",(long)gsmBuf, canGet,GSMPeer_fd);
		memset(Buf, 0, canRead);
		int rd = readFd(fd, Buf, canRead);
		if (rd > 0) {
			PutIncrement(rd);
		}		
		return rd;
	}
	return 0;	
}


int t_ring_buf::WriteTo(fileHandle fd)
{
	int canWrite = 0;
	char *Buf = GetAddress(&canWrite,size);
	
	if ((Buf != NULL) && (canWrite > 0)) {
//		fprintf(stderr,"gsmBuf: 0x%08lx, canGet:%d, fd:%d\n",(long)gsmBuf, canGet,GSMPeer_fd);
		int wr = writeFd(fd, Buf, canWrite);
		if (wr > 0) {
			GetIncrement(wr);
		}		
		return wr;
	}
	return 0;	
}


