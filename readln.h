/* 
 * Thomas Pantzer: readln.h, Version: v0.1a  16.02.2000 18:02 
 * 
 * 
 * Copyright 2000 by Thomas Pantzer
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








#ifndef _READLN_H
#define _READLN_H

#include "tns_util/porting.h"		// need fileHandle

class t_buffer {
public:
	long int filelen;	// grows with reads

	long int lines, lshifts, fsize;

	bool Init(char *fn);
	bool Init(char *fn, int bs);
	char *ReadLn(void);				// read until Linefeed
	long int Pos(void);
	long int Offs(void);
	void Done(void);	
	
	virtual int lowLevelRead(char *buf, int size);
		// blocking version
	
	virtual ~t_buffer() {
	}; 
private:
	fileHandle fd;
	int bufsize;
	long int offs;		// points to the ram-address for the next line
	long int eot;		// end of loaded text
	long int rd;
	bool fend;
	char *buffer;
};




#endif
