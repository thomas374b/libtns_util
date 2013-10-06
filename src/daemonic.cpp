/* 
 * Thomas Pantzer: daemonic.cc, v0.1b  08.05.2002 23:07 
 * 
 * 
 * Copyright 2002 by Thomas Pantzer
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
 * Thomas@Pantzer.net   (Thomas Pantzer,,,) 
 * 
 */


#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include "tns_util/porting.h"
#include "tns_util/daemonic.h"
#include "tns_util/timer.h"

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"



#if _WINDOWS | WIN32
int geteuid() {
	return 0;	
}
int getpid() {
	return 0;
}
int fork() {
	return 0;	
}
#endif

bool detached = false;
FILE *elogf = NULL;
FILE *slogf = NULL;



void open_errlog(const char *target)
{
#ifdef WITH_SYSLOG
	void openlog(target, 0, LOG_FACILITY);
#else		
	char st[1024];

	if (geteuid() == 0) {
		snprintf(st,768,"/var/log/%s/errors",target);
 	} else {
		snprintf(st,768,".%s-errors",target);
	}
	elogf = fopen(st,"a");

#if _WINDOWS | WIN32
#else
	// redirect old stdout and stderr to our new log file
	stderr = elogf;
	stdout = elogf;
#endif
#endif	
}

void close_errlog(void)
{
#ifdef WITH_SYSLOG
	closelog();
#else
	if (elogf != NULL) {
		fclose(elogf);
		elogf = NULL;
	}
#endif			
}


char *mk_logString(time_t now, bool humanReadableTime, char *msg, char *buffer)
{
	char ts[256];

	if (humanReadableTime) {
		TimeString(now, ts);
	} else {
		sprintf(ts,"%ld",now);
	}
    snprintf(buffer, 16384, "%s\t%s\n", ts, msg);

	return buffer;
}

// just pump(append) some bytes to a file
int writeOrAppend(char *filename, char *buffer)
{
	fileHandle fd = openFd(filename, O_WRONLY|O_APPEND);
	if (fd < 0) {
		switch (errno) {
			case ENOENT:
				fd = openFileMode(filename, O_WRONLY|O_CREAT, 0644);
				if (fd != INVALID_HANDLE_VALUE) {
					break;
				}
			default:
				EPRINTF1("open %s: %s\n", filename, strerror(errno));
				return -1;
		}
	}
	writeFd(fd, buffer, strlen(buffer));
	closeFd(fd);
	return 0;
}

void mk_log_entry2(char *filename, time_t now, bool plainFormat, char *msg)
{
	char s2[2048 + 128];
	writeOrAppend(filename, mk_logString(now, plainFormat, msg, s2));
}


void mk_log_entry(char *filename, time_t now, char *msg)
{
	mk_log_entry2(filename, now, true, msg);
}

void mk_log_entry_now(char *filename, char *msg)
{
	time_t now = time(NULL);
	mk_log_entry2(filename, now, false, msg);
}

void do_logging(const char *target, char *msg)
{
	char st[1024];
	if (geteuid() == 0) {
		snprintf(st,768,"/var/log/%s/.log",target);
	} else {
		snprintf(st,768,"%s.log",target);	
	}
	mk_log_entry(st,time(NULL),msg);
}


void write_pidfile(const char *target)
{
	if (geteuid() != 0) 
		return;
	
	if (target == NULL)
		return;
		
	char st[1024];
	snprintf(st,768,"/var/run/%s.pid",target);	
	fileHandle fd = openFileMode(st, O_RDWR|O_CREAT|O_TRUNC, 0644);
	if (fd < 0) {
		char s2[1024];
		sprintf(s2,"open(\"%s\")",st);
		perror(s2);
		return;
	}
	snprintf(st,768,"%d\n",getpid());	
	writeFd(fd,st,strlen(st));
	closeFd(fd);
}


void daemonize(const char *target)
{
#if _WINDOWS | WIN32
#else
	if (fork() == 0) { // child
		write_pidfile(target);
		
		close(0);
		close(1);
		close(2);
		detached = true;
	} else {	// parent
		exit(0);		
	}
#endif
}

