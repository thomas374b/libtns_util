/* 
 * Thomas Pantzer: daemonic.h, v0.1b  08.05.2002 23:07 
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





#ifndef _DAEMONIC_H
#define _DAEMONIC_H

#include <stdio.h>
#include <string.h>

#include <time.h>

extern bool detached;

extern FILE *elogf;			/* error logfile */
extern FILE *slogf;			/* standard logfile */
extern FILE *dlogf;			/* debug logfile */

extern void daemonize(const char *target);
extern void open_errlog(const char *target);
extern void close_errlog(void);
extern void do_logging(const char *target, char *msg);
extern void mk_log_entry(char *filename, time_t now, char *msg);
extern void mk_log_entry2(char *filename, time_t now, bool plainFormat, char *msg);
extern void mk_log_entry_now(char *filename, char *msg);

typedef void (*sighandler_t)(int);

#ifdef WITH_SYSLOG
	#include <syslog.h>
	
	#ifndef LOG_FACILITY
		#define LOG_FACILITY LOG_DAEMON
/*		#define LOG_FACILITY LOG_LOCAL0
		#define LOG_FACILITY LOG_LOCAL7
*/		
	#endif
#endif	



#ifdef TARGETNAME

	#ifdef WITH_SYSLOG
		#error still at work
/*	
		#define LPRINTF(y)	{									\
								if (detached == false) {		\
									char z[4096];				\
									snprintf(z,3072,y);			\
									do_logging(TARGETNAME,z);	\
								} else 									\
									syslog(LOG_INFO|LOG_FACILITY,y);	\
							}

		#define EPRINTF(xxx)	{										\
								if (detached == false)					\
									fprintf(stderr,xxx);				\
								else 									\
									syslog(LOG_ERR|LOG_FACILITY,xxx);	\
								}												
*/
	#else
		#define LPRINTF(fmt,args...)	{								\
							char z[4096];				\
							snprintf(z,3072,fmt,##args);			\
							do_logging(TARGETNAME,z);	\
						}

		#define EPRINTF(fmt,args...)	{									\
								if (detached == false) {				\
									fprintf(stderr,fmt,##args);			\
									fprintf(stderr,"\n");			\
								} else {								\
									if (elogf != NULL) {				\
										fprintf(elogf,fmt,##args); 		\
										fprintf(elogf,"\n");			\
										fflush(elogf);				\
									} else {							\
										char z[4096];				\
										snprintf(z,3072,fmt,##args);		\
										do_logging(TARGETNAME,z);	\
									}								\
								}									\
							}			
	#endif
#endif

#ifdef TRACE_ALL
	#define TRACE(x...)	EPRINTF(x)
#else
	#define TRACE(x...)
#endif



#endif // _DAEMONIC_H
