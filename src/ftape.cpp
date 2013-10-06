/* 
 * Thomas Pantzer: ftape.cc, v0.1b  08.05.2002 23:07 
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




#include "tns_util/ftape.h"
#include "tns_util/utils.h"

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"



void t_filetape::Init(int s, char *b, char *t, char *e)
{
	size = s;
	basepath = b;
	track = t;

	ext = new char *[16];
	
	ext[0] = e;
	ext_cnt = 1;

	pos = 0;
}

void t_filetape::Done(void)
{
	delete ext;
	ext_cnt = 0;
}


void t_filetape::AddExt(char *e)
{
	ext[ext_cnt] = e;
	ext_cnt++;
}


char *t_filetape::_check(char *buf, bool cond, int inc, int p)
{
	char *s = buf;
	int i = p;
	int j = 0;

	if (buf == NULL)
		s = new char[512]; 
	
	while (true) {
		int g = 0;
			
		for (int k=0; k<ext_cnt; k++) {
			sprintf(s,"%s/%s%d.%s",basepath,track,i,ext[k]);
			if (fexist(s) == cond) {
				if (cond == true) {
					pos = i+1;
					return s;
				} else {
					g++;
				}	
			}
		}
			
//		if (cond == true) 
//			pos = i+1;

		if (g == ext_cnt) {
			sprintf(s,"%s/%s%d.%s",basepath,track,i,ext[0]);
			return s;
		}

		i += inc;

		if (cond == true)  // Next() and Prev() will wrap @ size
			i %= size;

//		fprintf(stderr,"i,s,p: %d,%d,%d\n",i,size,p);

		if (i == 0) { // wraparound
			j++;
			if (j > 2) {  // no valid name found within two loops
				fprintf(stderr,"no file exists for mask \"%s\"\n",s);
				s[0] = '\0';
				return s;
			}
		}
	}
}

