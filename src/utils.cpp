/* 
 * University of Leipzig: utils.cc, v0.1b  21.9.97 22:58 
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


#include "tns_util/porting.h"
#include "tns_util/utils.h"
#include "tns_util/daemonic.h"

//#define SHOW_COMPILER_MOD 
#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"

#if _WINDOWS | WIN32
#else
	#include <dirent.h>
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#if _WINDOWS | WIN32
#else
// Kommentar eigentlich total überflüssig
int __max(int a, int b)		// could be a macro
{
  if (a>b)
     return a;
   else 
     return b;
}       

int __min(int a, int b)
{
  if (a<b)
     return a;
   else 
     return b;
}       
#endif

char unit_prefix[8] = " kMGTPE";

char *sprint_kM(double d, char *buf)	// print a number in %3.1f format with proper unit prefixes to stdout
{
	int i=0;
	double f = d;
  
	while (f > 1000.0)  {
		i++;
		f /= 1000.0;
	} 
    
	sprintf(buf,"%3.1f%c",f,unit_prefix[i]);  

	return buf;
}


void print_kM(double d)	// print a number in %3.1f format with proper unit prefixes to stdout
{
	char st[256];
	printf("%s",sprint_kM(d,st));
}



long power(int base, int exp)	// returns base^exp
{
   long s = 1;

   for (int i=0; i<exp; i++)
     s *= base;
  
   return s;
}


int log2(int n)
{
	int be = 0;
	while (n > 0)
	  {
	   n >>= 1;
	   be++;
	  }
	  
	return (be-1);	  
}


//#if _WINDOWS | WIN32
//#else
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ist wahr, wenn es aus der Datei des Filedescriptors 'was zu lesen gibt  
// gewartet wird ca. 10ms
//______________________________________________________________________
int FD_Ready(int fd)		// return true if input data available
{
	return FD_ReadyTo(fd, 10000);
}


int FD_ReadyTo(int fd, int to)	// same with user timeout in µsec
{
  fd_set Read_Set;
  struct timeval TimeOut;

  if (fd < 0) {
	   return false;	// not a valid file descriptor
  }
  FD_ZERO(&Read_Set);
  FD_SET(fd, &Read_Set);

  TimeOut.tv_sec=0;
  TimeOut.tv_usec=to;

//#ifdef hpux
//  select(fd+1,(int *)&Read_Set,0,0,&TimeOut);
//#else
  select(fd+1, &Read_Set, 0, 0, &TimeOut);
//#endif    

	if (FD_ISSET(fd,&Read_Set)) {
		return 1;
	} else {
		return 0;   
	}
}

int FD_Writeable(int fd)	// return true if fd is _IMMEDEATLY_ writeable
{
  fd_set Write_Set;
  struct timeval TimeOut;

  if (fd < 0) {
	   return false;	// not a valid file descriptor
  }

  FD_ZERO(&Write_Set);
  FD_SET(fd, &Write_Set);
  TimeOut.tv_sec=0;
  TimeOut.tv_usec=10000;

//#ifdef hpux
//  select(fd+1,(int *)&Read_Set,0,0,&TimeOut);
//#else
  select(fd+1,0,&Write_Set,0,&TimeOut);
//#endif    
  if (FD_ISSET(fd,&Write_Set)) {
       return 1;
  } else {
       return 0;   
  }
}
//#endif

bool fexist(char *s)		// return true if file "s" exists
{
	if (s == NULL)
	   return false;

	struct stat buf;

	if (stat(s,&buf) == -1) {
#ifdef DEBUG
		fprintf(stderr,"stat(%s): %s\n",s,strerror(errno));
#endif	
	   return false;
	}
	fileHandle fd = openFd(s,O_RDONLY); 
	if (fd < 0) {
#ifdef DEBUG
		fprintf(stderr,"open(%s): %s\n",s,strerror(errno));
#endif	
	   return false;
	}
	closeFd(fd);
	return true;		
}


char *strip_slash(char *argv0)
{
        char *s = strrchr(argv0,'/');

        if (s != NULL)
           return s+1;

        return argv0; 
}

long int filelen(char *s)		
{
	fileHandle fd = openFd(s,O_RDONLY);
	if (fd < 0)
	   return -1;
	   
	long int len = lseekFd(fd,0,SEEK_END);
	closeFd(fd);
	return len;
}


#if _WINDOWS | WIN32
#else
char test_dir(char *s)
{
   DIR *dir;
 
   if (s == NULL)
      return false;
   
   dir = opendir(s);
   if (dir != NULL)
      {
       closedir(dir);
       return true;
      } 

   return false;  
}
#endif

char strnrcmp(char *s1, char *s2, unsigned int len)
{
	if ((s1 == NULL) || (s2 == NULL))
	   return false;

	if ((strlen(s1) < len) || (strlen(s2) < len) )
	   return false; 

//	char st[256];
//	strcpy(st,s2);
	char *s3 = s2;
	s3 += strlen(s2);
	s3 -= len;

#ifdef DEBUG
	fprintf(stderr,">%s< >%s<\n",s1,s3);
#endif
	if (strncmp(s1,s3,len) == 0)
	   return true;

	return false;
}




int fileRead(const char *fn_arg, char *buffer, int len)
{
	int fd = open(fn_arg, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "%s:%d: can't open file [%s], %s, %d\n", __FILE__, __LINE__, fn_arg, strerror(errno), errno);
    	return -errno;
	}
	int r = read(fd, buffer, len );
	close(fd);
	if (r == len) {
	    fprintf(stderr, "not all bytes read, buffer exhausted @ %d\n", len);
	}
    return r;
}

int fileReadInt(const char *fn)
{
	char buffer[128];
	memset(buffer, 0, 128);

	int len = fileRead(fn, buffer, 128);
	if (len < 0) {
		return len;
	}
	int result = 0;
	int n = sscanf(buffer, "%d", &result);
	if (n == 1) {
		return result;
	}
	fprintf(stderr, "scan result of [%s] = %d, len:%d\n", buffer, n, len);
	return -1;
}





int getIntFromEnv(const char *name, int defaultValue)
{
	char *v = getenv(name);
	if (v == NULL) {
		return defaultValue;
	}
	int d;
	if (sscanf(v, "%d", &d) == 1) {
		EPRINTF("overriding defaults for %s from user settings to %d", name, d);
		return d;
	}
	return defaultValue;
}

float getFloatFromEnv(const char *name, float defaultValue)
{
	char *v = getenv(name);
	if (v == NULL) {
		return defaultValue;
	}
	float d;
	if (sscanf(v, "%f", &d) == 1) {
		EPRINTF("overriding defaults for %s from user settings to %f", name, d);
		return d;
	}
	return defaultValue;
}


bool getBoolFromEnv(const char *name, bool defaultValue)
{
	char *v = getenv(name);
	if (v == NULL) {
		return defaultValue;
	}
	char yesIndices[] = "1yYjJ";
	char noIndices[] = "0nN";

	for (uint8_t i=0; i<5; i++) {
		char *y = strchr(v, yesIndices[i]);
		if (y != NULL) {
			EPRINTF("overriding defaults for %s from user settings to true", name);
			return true;
		}
	}
	for (uint8_t i=0; i<3; i++) {
		char *y = strchr(v, noIndices[i]);
		if (y != NULL) {
			EPRINTF("overriding defaults for %s from user settings to false", name);
			return false;
		}
	}
	return defaultValue;
}

