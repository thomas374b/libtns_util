/* 
 * Thomas Pantzer: t_floating_avg.cc, v0.1b  8.6.99 16:49 
 * 
 * 
 * Copyright 1999 by Thomas Pantzer
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
 * pantec@isys2.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */

#undef DEBUG	// define this if assumed to be bugfree
// #define DEBUG
// #define DEBUG_ALL


#include <math.h>
#include <stdio.h>
#include <string.h>

#include "tns_util/porting.h"
#include "tns_util/t_floating_avg.h"
#include "tns_util/daemonic.h"


#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"


 

t_NamedObj::t_NamedObj()
{
	memset(ItsName,0,64); 
	Sptr = NULL;
}

void t_NamedObj::SetSubPointer(t_NamedObj **x)
{
	Sptr = x;
}


void t_NamedObj::SetName(const char *name)
{
	if (name == NULL) {
		return;
	}
	snprintf(ItsName,62,"%s",name);
}


bool t_NamedObj::Save(fileHandle fd)
{
	char buf[4096];
	Printf(buf);
	strcat(buf,"\n");
	int len = strlen(buf);
	int w = writeFd(fd,buf,len);
	return (w == len);	 
}

bool t_NamedObj::Load(fileHandle fd)
{
#ifdef DEBUG
	TRACE("%s::Load(%s)\n",getClassName(),getName());
#endif
	char buf[4096];
	memset(buf,0,4096);
	
	int r = readFd(fd,buf,4096-16);
	if (r <= 0)  {
		return false;
	}
	
	char test[] = {'\n','\t',' '};
	for (int i=0; i<3; i++) {
		char *s = strchr(buf,test[i]);
		while (s != NULL) {
			char *s2 = s;
			s++;
			int len = strlen(s);
			memmove(s2,s,len);
			s[len-1] = '\0';
			
			s = strchr(buf,test[i]);
		}
	}
	
	return Scanf(buf);	
}


bool t_NamedObj::Scanf(char *buf)
{
#ifdef DEBUG
	TRACE("%s::Scanf(%s)\n",getClassName(),getName());
#endif
	if (buf == NULL) {
		EPRINTF1("buffer is NULL on %s/%s::Scanf()",getName(),getClassName());
		return false;
	}		
	int sl = strlen(buf);
	if (sl < 3) {
		EPRINTF1("buffer too short on %s/%s::Scanf()",getName(),getClassName());
		return false;
	}
	char *c_end = &buf[sl];
	char fmt[1024];
#ifdef DEBUG
	TRACE("processing %s/%s\n",getName(),getClassName());
#endif
	if (Sptr == NULL) {
		EPRINTF0("Sptr is NULL");
		return false;
	}
	
	char *next = buf+1;
	int i=0;

	t_NamedObj *Ptr = Sptr[i];
	
	while (Ptr != NULL) {
#ifdef DEBUG_ALL
		EPRINTF("0x%08lx, %d\n",(long)Ptr,i);
		EPRINTF("0x%08lx\n",(long)Ptr->getName());
#endif
#ifdef DEBUG		
		EPRINTF("scanning _%s_ from %s\n",Ptr->getName(),getName());
#endif
		sprintf(fmt,"}%s",Ptr->getName());
		char *s = strstr(next,fmt);
		if (s == NULL) {
#ifdef DEBUG
			EPRINTF("delimiter _%s_ not found in _%s_\n",fmt,next);
#endif			
			return false;
		}
		char substr[1024];
		
		s[strlen(fmt)] = '\0';	
		strcpy(substr,next);
#ifdef DEBUG	
		EPRINTF("%d %s\n",i,Ptr->getName());	
#endif		
		if (Ptr->Scanf(substr) == false) {
#ifdef DEBUG
			EPRINTF("%s->Scanf(\"%s\") failed\n",Ptr->getName(),substr);
#endif			
			return false;
		} 
		next = &s[strlen(fmt)+1];

#ifdef DEBUG
		EPRINTF("%s->Scanf(\"%s\") succeeded,\ncontinuing with \"%s\"\n",Ptr->getName(),substr,next);
#endif			
		i++;
		Ptr = Sptr[i];
	}
	return true;
}



//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//		t_range
//
//_______________________________________________
void t_Range::Reset(void)
{
//	fprintf(stderr,"t_Range::Reset()\n");

	max = -1000000000.0;
	min = -max;
	size = 1.0;
	changed = 0;
	freezeBoundaries = false;
}

bool t_Range::Update(double d)
{
//	fprintf(stderr,"t_Range::Update()\n");
    
	changed = 0;
	last = d;
	
	if (d > max) {
		max = d;
		changed = 1; 
	}
	if (d < min) {
		min = d;
		changed = -1; 
	}	
	size = max - min;
	if (size == 0.0) {
		size = 1.0;
	}
	if (changed != 0) {
		if (!freezeBoundaries) {
			bias = - ((max + min) / 2.0);
			fullScale = size / 2.0;
		}
#ifdef DEBUG_ALL
		fprintf(stderr,"t_Range::Update(%g) %g<x<%g, c:%d\n", d, min, max, changed, bias, fullScale);
#endif
	}
	return (changed != 0);
}

bool t_Range::HasChanged(void)
{
	if (changed != 0) {
		changed = 0;
		return true;
	}
	return false;
}

// return value between {0..1}
double t_Range::scaled(void)
{
	if (size != 0.0) {
		return (last - min)/size;
	}		
	return 1.0;
}

// return value between {-1..1}
double t_Range::Scaled11(double x)
{
	if (size != 0.0) {			
		return (x + bias) / fullScale;
	}		
	return 1.0;
}

void t_Range::setBias(double b)
{
	bias = b;
	freezeBoundaries = true;
}

void t_Range::setFullScale(double fs)
{
	fullScale = fs;
}


short t_Range::scaled2(void)
{
	if (size != 0.0) {
		double D = ((last - min)/size) -0.5000;
		return (short)(D*32767.0);	
	}		
	return 0;
}

char *t_NamedRange::Printf(char *buf)
{
	if (buf != NULL) {
//		char st[256];
		sprintf(buf,"{min:%g, max:%g}%s",min,max,getName());
	}		
	return buf;
}


bool t_NamedRange::Scanf(char *st)
{
	if (st == NULL) {
		return false;
	}
#ifdef DEBUG
	TRACE("%s %s::t_NamedRange::Scanf(%s)\n",getName(),getClassName(),st);
#endif
	float i,a;
	char buf[256];
	if (sscanf(st,"{min:%g, max:%g}%s",&i,&a,buf) == 3) {
		if (strcmp(buf,getName()) == 0) {
			min = i;
			max = a;
			size = max - min;
			return true;
		}			
	}
	return false;
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//		t_laverage
//
//_______________________________________________
double t_laverage::updateGradient(double v)
{
	delta = (v - AvgV);
	gradient = (delta / alen);

	return gradient;
}

void t_laverage::Add(double v)
{
	t_Range::Update(v);
	AvgV += updateGradient(v);
}

double t_laverage::Range(void)
{
	return size;
}

void t_laverage::Reset(void)
{
	AvgV = 0.0;
	t_Range::Reset();
}


//_________________________________________________________
//
char *t_NamedAverage::Printf(char *buf)
{
	if (buf != NULL) {
//		char st[256];
		sprintf(buf,"{avgv:%g, len:%g, min:%g, max:%g}%s", AvgV, alen, min, max, getName());
	}		
	return buf;
}

bool t_NamedAverage::Scanf(char *st)
{
	if (st == NULL) {
		return false;
	}
#ifdef DEBUG
	TRACE("%s %s::t_NamedAverage::Scanf(%s)\n",getName(),getClassName(),st);
#endif
	float a,l,i,x;
	char buf[256];
	if (sscanf(st,"{avgv:%g, len:%g, min:%g, max:%g}%s",&a,&l,&i,&x,buf) == 5) {
		if (strcmp(buf,getName()) == 0) {
			AvgV = a;
			alen = l;
			min = i;
			max = x;
			size = max - min;
			return true;
		}			
	}
	return false;
}

void t_NamedSensorAverage::Add(double v, bool bWarmup)
{
	if (bWarmup) {
		AvgV = v;		// directly set average		// TODO: review this, use freezing-AVG instead
	} else {
		t_NamedAverage::Add(v);	// calculate average, update range
	}
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//		t_average
//
//_______________________________________________
bool t_average::_frozen(void)
{
	return (cnt > windowLength);
}

void t_average::Add(double v)
{
	updateGradient(v);

	if (_frozen()) {
		AvgV += gradient;
	} else {
		t_freezing_avg::Add(v);
		AvgV = t_freezing_avg::Value();
	}

	if (autoCalibration) {
	    Update(AvgV);	// expand ranges slowly
	} else {
	    Update(v);		// quickly expand ranges
	}    
}

void t_average::setCalibrationLen(double l)
{
	alen = l;
	autoCalibration = true;
	Reset();
}

void t_average::calibrate(double min, double max)
{
	Reset();
	autoCalibration = false;
	freezeBoundaries = false;
	Update(min);
	Update(max);
	freezeBoundaries = true;
	AvgV = (max+min)/2;
}



void t_average::finishCalibration(void) 
{
	alen = windowLength;
	autoCalibration = false;
	freezeBoundaries = true;

	fprintf(stderr,"fullScale: %f\n", fullScale);
}

void t_average::Init(double l)
{
	alen = l;
	windowLength = alen;
	t_laverage::Reset();
}

double t_average::Filtered(double fs)
{
    return t_Range::Scaled11(AvgV) * fs;
}














/**/
/**/
/**/
/**/
/**/
/**/
/**/
/**/
/*****  End Of File  ****************************/
