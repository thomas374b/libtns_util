/* 
 * University of Leipzig: timer.h, v0.1b  21.9.97 22:58 
 * 
 * 
 * Copyright 1997 by University of Leipzig
 * 
 * Permission to use, copy, modify, and distribute this software for noncommercial 
 * use and without fee is hereby granted, provided that the above copyright notice 
 * appear in all copies and that both that copyright notice and this permission 
 * notice appear in supporting documentation, and that the name of the U.L. not be 
 * used in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. The U.L. makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is" 
 * without expressed or implied warranty. 
 * 
 * 
 * pantec@aix520.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */



#ifndef _TIMER_H
#define _TIMER_H


// #include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>



extern long int GetTime(void);
extern void InitTimer(void);
extern void DoneTimer(void);
extern char *timepostfix(double milliseconds, char *buffer);
extern char *TimeString(time_t now, char *buffer);


#ifdef OLDSTYLE_TIMER
	typedef void (*sighandler_t)(int);

	extern void InitTimer(sighandler_t handler, long isec, long iusec);

	extern int timer_resolution;
	extern char timer_rolled;
#endif

#ifndef linux
	#ifdef sun
		/* problem was solved
		// I can't find the header file for this, but it exists
		extern "C" {
			extern void usleep(unsigned int);
		}
		*/
	#else 
		#ifdef hpux
			extern void usleep(long int i);
		#endif
	#endif
#endif

//globale Variablen für den Timer
//extern struct itimerval SavedTimer;


#endif

