/* 
 * Thomas Pantzer: readln.cc, Version: v0.1a  16.02.2000 18:02 
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



#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tns_util/porting.h"
#include "tns_util/readln.h"
#include "tns_util/utils.h"
#include "tns_util/daemonic.h"

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"


bool t_buffer::Init(char *fn, int bs)
{
	bufsize = 0;
	buffer = NULL;

	rd = 0;
	offs = 0;
	filelen = 0;
	lines = 0;
//	nchars = 0;
	eot = 0;
	fend = false;
	fsize = 0;
	lshifts = 0;

	if ((fn == NULL) || (strlen(fn) == 0)) {
		fd = 0;				
		fsize = -1;
	} else {	
		fd = openFd(fn,O_RDONLY);

		if (fd < 0)	   {
			char s2[1024];
			sprintf(s2,"t_buffer::Init(%s,%d)",fn,bs);
		    perror(s2);		
		    return false;
		} 
		fsize = lseekFd(fd,0,SEEK_END);
		lseekFd(fd,0,SEEK_SET);
	}	
	bufsize = bs;
	buffer = new char[bufsize+4096];

	memset(buffer, 0, bufsize+4096);

	return true;
}	


bool t_buffer::Init(char *fn)
{
//	int bs;
	struct stat stat_buf;	
	// TODO: recall filename in member variable
	
	int r = stat(fn,&stat_buf);
	if (r != 0) {
		fprintf(stderr,"can't stat(%s): %s\n",fn,strerror(errno));
		return false;
	}
#if _WINDOWS | WIN32
#else
	if (S_ISDIR(stat_buf.st_mode)) {
		fprintf(stderr,"\"%s\" is a directory\n",fn);
		return false;
	}
#endif
	fprintf(stderr,"sizeof \"%s\" is %ld\n",fn,stat_buf.st_size);
	return t_buffer::Init(fn, (stat_buf.st_size*2)+2048);	
}

long int t_buffer::Pos(void)
{
	if ((fsize == -1) || (fd == 0))
		return 0;
	
	return lseekFd(fd,0,SEEK_CUR);
}

long int t_buffer::Offs(void)
{
	return offs;
}


int t_buffer::lowLevelRead(char *_b, int _s)
{	
	return readFd(fd,_b,_s);
}

char *t_buffer::ReadLn(void)
{
	char *s = &buffer[offs];

	if (rd > 0) {
		int zc = 0;
		// skip zeros from text input
		while (*s == '\0') {
			offs++;
			zc++;
			if (offs < bufsize) {
				s = &buffer[offs];
			} else {
				if (filelen < fsize) {
					// need to reload buffer from disk
					eot = offs;
					break;
				} else {
					return NULL;
				}
			}
		}
		if (zc > 0) {
			// TODO: include filename in output
//			EPRINTF("skipped %d zero bytes in text-input", zc);
		}
	}

	char *nl = strchr(s,'\n');

	long int no = offs + ((nl - s)  + 1); 

	if ((nl != NULL) && (no < eot))  {
		offs = no;
		*nl = '\0';
		lines++;	
		return s;
	}

	if (fend == true) {
		return NULL;	
	}
	lshifts++;
#ifdef DEBUG_
	fprintf(stderr,"t_buffer::ReadLn() offs:%ld eot:%ld rd:%ld flen:%ld fsize:%ld\n",
							offs, eot, rd, filelen, fsize);
#endif
	if (eot >= offs) {

		int loadAddr = eot - offs;
		int toload = (bufsize - loadAddr);

		if (offs == 0) {
			if ((toload == 0) && (rd < fsize)) {
//				fprintf(stderr,"correction needed toload: %d rd: %ld, laddr: %d\n", toload, rd, loadAddr);

				toload = __min(fsize - filelen, bufsize);
				loadAddr = 0;
			}
		} else {
			for (long int i=offs; i<eot; i++)
				buffer[i-offs] = buffer[i];
		}

//		fprintf(stderr,"toload: %d rd: %ld, laddr: %d\n", toload, rd, loadAddr);
		rd = lowLevelRead(&buffer[loadAddr], toload);

		if (rd == 0) {
			if (errno != 0) {
				if (filelen != fsize) {
					EPRINTF("t_buffer::ReadLn().read(%d,&buffer[%d],%d) = %ld failed after %ld/%ld bytes (%ld/%ld lines), error:%d %s",
								fd, loadAddr, toload, rd, filelen, fsize, lines, lshifts, errno, strerror(errno) );
//				} else {
//					normal exit at end of file
//					errno==2 on banana
				}
			}
			eot = loadAddr+5;
			offs = 0;

			for (int i=0; i<8; i++) {
				buffer[eot +i] = '\n';
				buffer[eot +i + 8] = '\0';	
			}
			fend = true;

			return ReadLn();
		}	

	    if (rd > 0) {
			offs = 0;
			filelen += rd; 	
			eot = loadAddr + rd;

			memset(&buffer[eot], 0, 64);

			return ReadLn();
		}

		EPRINTF("!!! readln.cpp: t_buffer::ReadLn().read(%d,&buffer[%d],%d) = %ld failed after %ld bytes (%ld/%ld lines), error:%d %s",
						fd, loadAddr, toload, rd, filelen, lines, lshifts, errno, strerror(errno) );
	} else {	
		EPRINTF("!!! readln.cpp: t_buffer::ReadLn(): should not happen (%ld,%ld,%ld)", rd, eot, offs);
	}	
	return NULL;
}



void t_buffer::Done(void)
{
	if (fd != INVALID_HANDLE_VALUE) {
		closeFd(fd);
	}
	delete buffer;
	buffer = NULL;
}











