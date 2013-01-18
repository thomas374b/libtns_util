/* 
 * Thomas Pantzer: timer.cc, v0.1b  24.11.98 10:27 
 * 
 * 
 * Copyright 1998 by Thomas Pantzer
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
 * pantec@aix520.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */




#include "tns_util/timer.h"
#include "tns_util/daemonic.h"

//#define SHOW_COMPILER_MOD 
#include "tns_util/copyright.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

#ifdef AIX
	extern "C" {
	extern int setitimer (int Which, struct itimerval *Value, struct itimerval *OValue);
  	extern int getitimer (int Which, struct itimerval *Value);
	}
#endif  



#ifndef OLDSTYLE_TIMER

long start_time = 0;

long _get_time(void)
{
	timeval tv;
	gettimeofday(&tv, (struct timezone *)NULL);
	return ((tv.tv_sec*1000) + (tv.tv_usec / 1000));
}

long GetTime(void)
{
	return (_get_time() - start_time);
}

void InitTimer(void)
{
	start_time = _get_time();
}

void DoneTimer(void)
{
	fprintf(stderr,"fix me, DoneTimer() is obsoleted\n");
}

#else

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// initialisiert und startet den Timer
//___________________________________

static itimerval SavedTimer;

char timer_rolled = false;
int timer_resolution;

sighandler_t old_handler;


#ifndef TIMER_OVERFLOW
#define TIMER_OVERFLOW	360000			// max. ist MAXLONGINT/1000
#endif

#ifdef hpux
  // (HP) hat eine minimale Timerauflösung von 10 msec.
  #define MinTimeWait 10000

  // deshalb benötigt man noch ein paar Hilfsvariablen 
  static long int not_waited;
#endif

struct itimerval _the_Timer;



void MyTimerHandler(int i)
{
//   signal(SIGALRM,old_handler);
   signal(SIGALRM,&(MyTimerHandler));  

//   old_handler(i);
   // fprintf(stderr,"Handler called with [%d]\n");
   timer_rolled = true;
}


void InitTimer(sighandler_t handler,long isec, long iusec)
{
  _the_Timer.it_value.tv_sec=0;
  _the_Timer.it_value.tv_usec=1;       //  start nach 1µs
  _the_Timer.it_interval.tv_sec=isec;     //  100h bis Timerüberlauf
  _the_Timer.it_interval.tv_usec=iusec;

  old_handler = signal(SIGALRM,handler);  
#ifdef DEBUG
//  fprintf(stderr,"old_handler: 0x%lx\n",(long)old_handler);
#endif
  setitimer(ITIMER_REAL,&_the_Timer,&SavedTimer);
  
#ifdef hpux
  not_waited=0; 
#endif    
  
  long int L1,L2; 

  do { 
      L1 = GetTime(); 
     } while (timer_rolled == false);	

  L1 = GetTime();
  do {
      L2 = GetTime();
     } while (L2 == L1);


  timer_rolled = false;
  timer_resolution = (int)(L2-L1);
#if (DEBUG || VERBOSE)
//  fprintf(stderr,"timer resolution: %dms\n",timer_resolution);
#endif

}

void InitTimer(void)
{
	InitTimer(&MyTimerHandler,TIMER_OVERFLOW,0);
}

void DoneTimer()
{
  struct itimerval Dummy;

  setitimer(ITIMER_REAL,&SavedTimer,&Dummy);
}  


//gibt die Anzahl der verstrichenen Millisekunden zurück  
long int GetTime(void)
{
  long int Time;
  
  struct itimerval T;
  getitimer(ITIMER_REAL,&T);

  Time = (TIMER_OVERFLOW*1000);
  Time -= (T.it_value.tv_sec*1000);
  Time -= (T.it_value.tv_usec/1000);

//  *Time=3600000 - T.it_value.tv_sec;
  return Time;  
}

#endif


#ifdef hpux 

//  (!(linux || sun))

void usleep(long int i)
{
  long int k;
  struct itimerval	T,O,D;

//  not_waited=not_waited+i;
  k=i;
  
  while (k>0)
     {
      k=k-MinTimeWait;     // HP's können nur im 10ms-Takt warten  ( µs wären besser )
                     
      getitimer(ITIMER_REAL,&O);
      // hier wird CPU-Takt-Verschwendend gewartet  
      // Grüße an HP -- wo ist der usleep()-Systemruf
      do {
          getitimer(ITIMER_REAL,&T);
         } while (T.it_value.tv_usec==O.it_value.tv_usec);
     }
     
  // das naechste mal machen wir das mit "select()"   
}



void usleep_v2(long int i)
{
  struct itimerval T,O,D;
  long int k;

  T.it_value.tv_sec=0;
  T.it_value.tv_usec=i;       //  start nach 1ms
  T.it_interval.tv_sec=0;     
  T.it_interval.tv_usec=0;

  getitimer(ITIMER_REAL,&O);
  signal(SIGALRM,&(MyTimerHandler));  
  setitimer(ITIMER_REAL,&T,&D);
  pause();

  if (O.it_value.tv_usec>i)
        O.it_value.tv_usec=O.it_value.tv_usec-i;             
     else 
       {
        k=i-O.it_value.tv_usec;
        O.it_value.tv_usec=1000000-k;             
        O.it_value.tv_sec--;
       }   

  O.it_interval.tv_sec=3600;     //  1h bis Timerüberlauf
  O.it_interval.tv_usec=0;

  signal(SIGALRM,&(MyTimerHandler));  
  setitimer(ITIMER_REAL,&O,&D);
}



void usleep_v1(long int i)
{
  long int j,k;
  struct itimerval	T;

  getitimer(ITIMER_REAL,&T);
  
  if (T.it_value.tv_usec>i)
       {
        j=T.it_value.tv_usec-i;
      
        while (T.it_value.tv_usec>j)
          getitimer(ITIMER_REAL,&T);
       }  
    else  
       {
        k=T.it_value.tv_usec;
        j=k+1;
        while (k<j)
          {
           getitimer(ITIMER_REAL,&T);
           j=k;
           k=T.it_value.tv_usec;
          } 

        j=1000000-(i-T.it_value.tv_usec);
        while (T.it_value.tv_usec>j)
          getitimer(ITIMER_REAL,&T);
       }
}

#endif



typedef struct {
	char unit[64];
	double divider;
} t_scaler;


t_scaler TimeScale[] = {
	{"msec",1.0},
	{"seconds",1000.0},
	{"minutes",60.0},
	{"hours",60.0},
	{"days",24.0},
	{"weeks",7.0},
	{"months",0.0},
};

char *timepostfix(double rto, char *buffer)
{
	double dt = rto;

	int i = 0;
	sprintf(buffer,"% 3.2f %s",dt,TimeScale[i].unit);
	i++;
	while ((dt > TimeScale[i].divider) && (TimeScale[i].divider > 0.0)) {	
		dt /= TimeScale[i].divider;
		sprintf(buffer,"% 3.2f %s",dt,TimeScale[i].unit);
		i++;
	}	
/*
	sprintf(buffer,"% 3.2f msec",dt);
	if (dt > 1000.0) {
		dt /= 1000.0;
		sprintf(buffer,"% 3.2f seconds",dt);
		if (dt > 60.0) {
			dt /= 60.0;
			strcpy(buffer,"minutes\0");
			if (dt > 60.0) {
				dt /= 60.0;
				strcpy(buffer,"hours\0");
				if (dt > 24.0) {
					dt /= 24.0;
					strcpy(buffer,"days\0");
					if (dt > 7.0) {
						dt /= 7.0;
						strcpy(buffer,"weeks\0");
					}	
				}	
			}	
		}	
	}
*/	
	return buffer;	
}


char *TimeString(time_t now, char *buffer)
{
	struct tm  *TM = localtime(&now);
	if (buffer != NULL) {
		strftime(buffer,768,"%Y/%m/%d %H:%M:%S",TM);
	}	
	return buffer;
}

/*__________________________________

	End-Of-File
  __________________________________*/

