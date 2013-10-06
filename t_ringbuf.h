/* 
 * Thomas Pantzer: t_ringbuf.h, v0.1b  6.8.99 16:10 
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







#ifndef T_RINGBUF_H
#define T_RINGBUF_H


#include "tns_util/porting.h"		// need fileHandle

// implement a ringbuffer for blockwise IO with 
// different in- and out- blocksizes
// if a blockwrite exceeds the end of the buffer,
// the overflow portion is copied to the start area of the buffer
// 

class t_ring_buf {
private:
	char *rptr,*wptr,*Array,*tmp_wptr,*hangover_ptr;
	int size,real_size,get_size,put_size;
	int hangover;
	char *PutAddress(int *avail, int threshold);
	
public:
	t_ring_buf() {
		tmp_wptr = hangover_ptr = 0;
		hangover = 0;
	};
	
	long RPtr(void) { return (rptr - Array); };
	long WPtr(void) { return (wptr - Array); };
	
	void Init(int len, int p_size, int g_size);
				// len must be multiple of get_size
	void Done(void);
	char *Base(void) { return Array; };
	void Reset(void) { 
		rptr = &Array[0]; 
		wptr = &Array[0];
		tmp_wptr = hangover_ptr = 0;
		hangover = 0;
	};

	virtual int Get_Avail(void);	// size of largest block until end of buffer (wrap)
	virtual int Put_Avail(void);
	
	char *GetAddress(int *free,int request);		// return address where to get
	char *GetBlockAddress(int *free);
	void GetIncrement(void);
	void GetIncrement(int step);

//	void GetBack(int steps);

	char *PutAddress(int *avail) { return PutAddress(avail,1); };
		// return address where to put and free space

	char *PutBlockAddress(int *avail) { return PutAddress(avail,put_size); }; 
		// return address where to put and free space
		
	void PutIncrement(int inc);	
	void PutIncrement(void) { PutIncrement(put_size); };	

	int Put(char *buf, int s);	
	int Put(char *buf) { return Put(buf,put_size); };	// memcpy

	bool bGetAvail(void);
	char GetSingle(void);

//	void PutBack(int steps);
	void PutWrap(void);
	void GetWrap(void);
	
	int ReadFrom(fileHandle fd);
	int WriteTo(fileHandle fd);
};




#endif
