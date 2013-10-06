/* 
 * Thomas Pantzer: t_Glimpselog.cpp, Version: v0.1a  22.04.2010 13:46 
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



#include <fcntl.h>
#include <errno.h>

#include "tns_util/porting.h"
#include "tns_util/t_Glimpselog.h"
#include "tns_util/daemonic.h"
#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"

#ifndef MAXSTRLEN
#define MAXSTRLEN	4096
#endif

void t_GlimpseLog::Open(char *target)
{
	snprintf(logfile,768,"%s",target);
	fd = openFd(logfile, O_RDWR);
	if (fd < 0) {
		switch (errno) {
			case ENOENT:
				fd = openFileMode(logfile, O_WRONLY|O_CREAT, 0644);
				if (fd != INVALID_HANDLE_VALUE) {
					break;
				}		
			default:
				EPRINTF1("open %s: %s\n", logfile, strerror(errno));
				return;      
		}				
	}
	if (offset == -1) {
		offset = lseekFd(fd, 0, SEEK_END);
		if (offset == -1) {
			EPRINTF("lseek to end: %s\n",strerror(errno));
			offset = 0;
		}
	}		
}
 
void t_GlimpseLog::Write(char *msg)
{
	if (offset == -1) {
		EPRINTF0("call t_GlimpseLog::Open(...) first\n");
		return;
	}
	fd = openFd(logfile, O_RDWR);
	
	if (fd == INVALID_HANDLE_VALUE) {
		EPRINTF1("open file %s: %s\n",logfile,strerror(errno));
		return;
	}
	int l = lseekFd(fd, offset, SEEK_SET);
	if (l == -1) {
		EPRINTF1("lseek to %d: %s\n",offset,strerror(errno));
	}
	
	char st[MAXSTRLEN];		// pad last bytes with white space
	memset(st,0,MAXSTRLEN);
	strcat(st,msg);
	
	int len = strlen(st);
	if (maxlen < len) {
		maxlen = len;
	}
	int i = maxlen - len;
	for (int j=0; j<i; j++) {
		strcat(st," ");	
	}		
	int l2 = strlen(st);
	strcpy(lastlogstring, st);
	int w = writeFd(fd,st,l2);
	if (w != l2) {
		EPRINTF4("error:%d, only %d of %d bytes written: %s, maxlen: %d\n",errno,w,strlen(st),strerror(errno),maxlen);
	}
	closeFd(fd);
}

void t_GlimpseLog::Advance(char *msg)
{
	Advance();
	int forward = strlen(msg);
	Write(msg);
	offset += forward;
}


void t_GlimpseLog::Advance(void)
{
	int forward = strlen(lastlogstring);
	offset += forward;
}

