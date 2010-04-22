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


#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "tns_util/t_Glimpselog.h"
#include "tns_util/daemonic.h"

//#define SHOW_COMPILER_MOD 
#include "tns_util/copyright.h"


void t_GlimpseLog::Open(char *target)
{
	snprintf(logfile,768,"%s",target);
	fd = open(logfile,O_RDWR);
	if (fd < 0) {
		switch (errno) {
			case ENOENT:
				fd = open(logfile,O_WRONLY|O_CREAT,0644);
				if (fd != -1) {
					break;
				}		
			default:
				EPRINTF("open %s: %s\n",logfile,strerror(errno));
				return;      
		}				
	}
	if (offset == -1) {
		offset = lseek(fd,0,SEEK_END);
		if (offset == -1) {
			EPRINTF("lseek to end: %s\n",strerror(errno));
			offset = 0;
		}
	}		
}
 
void t_GlimpseLog::Write(char *msg)
{
	if (offset == -1) {
		EPRINTF("call t_GlimpseLog::Open(...) first\n");
		return;
	}
	fd = open(logfile,O_RDWR);
	
	if (fd == -1) {
		EPRINTF("open file %s: %s\n",logfile,strerror(errno));
		return;
	}
	int l = lseek(fd,offset,SEEK_SET);
	if (l == -1) {
		EPRINTF("lseek to %d: %s\n",offset,strerror(errno));
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
	int w = write(fd,st,l2);
	if (w != l2) {
		EPRINTF("error:%d, only %d of %d bytes written: %s, maxlen: %d\n",errno,w,strlen(st),strerror(errno),maxlen);
	}
	close(fd);
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

