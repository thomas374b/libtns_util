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




#include "tns_util/daemonic.h"
#include "tns_util/timer.h"

//#define SHOW_COMPILER_MOD 
#include "tns_util/copyright.h"


#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>


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

	// redirect old stdout and stderr to our new log file
	stderr = elogf;
	stdout = elogf;
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



void mk_log_entry2(char *filename, time_t now, bool plainFormat, char *msg)
{
	char ts[1024];
	char s2[2048 + 128];  
	
	int fd = open(filename,O_WRONLY|O_APPEND);

	if (fd < 0) {
		switch (errno) {
			case ENOENT:
				fd = open(filename,O_WRONLY|O_CREAT,0644);
				if (fd != -1) {
					break;
				}		
			default:
				EPRINTF("open %s: %s\n",filename,strerror(errno));
				return;      
		}				
	}
	if (plainFormat) {
		TimeString(now, ts);
	} else {
		sprintf(ts,"%ld",now);
	}
		
    snprintf(s2,2048,"%s\t%s\n", ts, msg);

	write(fd,s2,strlen(s2));	    	
	close(fd);
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
	int fd = open(st,O_RDWR|O_CREAT|O_TRUNC,0644);
	if (fd < 0) {
		char s2[1024];
		sprintf(s2,"open(\"%s\")",st);
		perror(s2);
		return;
	}
	snprintf(st,768,"%d\n",getpid());	
	write(fd,st,strlen(st));
	close(fd);
}

void daemonize(const char *target)
{
	if (fork() == 0) { // child
		write_pidfile(target);
		
		close(0);
		close(1);
		close(2);
		detached = true;
	} else {	// parent
		exit(0);		
	}
}
