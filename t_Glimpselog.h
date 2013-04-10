/* 
 * Thomas Pantzer: t_Glimpselog.h, Version: v0.1a  22.04.2010 13:46 
 * 
 * 
 * Copyright 2010 by Thomas Pantzer
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
 * pantec@doha.private.site   (Thomas Pantzer,,,) 
 * 
 */

#ifndef _T_GLIMPSELOG_H
#define _T_GLIMPSELOG_H


class t_GlimpseLog {
private:
	int fd,offset,maxlen;
	char logfile[512];
	char lastlogstring[2048];
public:
	t_GlimpseLog() {
		fd = -1;
		offset = -1;
		maxlen = 0;
		lastlogstring[0] = '\0';	
	};
	void Open(char * target);
	void Write(char * msg);
	void Advance(char * msg);
	void Advance(void);
	int getFD(void) {return fd;};
};


#endif
