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

#include "tns_util/t_floating_avg.h"

//#define SHOW_COMPILER_MOD 
#include "tns_util/copyright.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
 

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


bool t_NamedObj::Save(int fd)
{
	char buf[4096];
	Printf(buf);
	strcat(buf,"\n");
	int len = strlen(buf);
	int w = write(fd,buf,len);
	return (w == len);	 
}

bool t_NamedObj::Load(int fd)
{
	TRACE("%s::Load(%s)\n",getClassName(),getName());

	char buf[4096];
	memset(buf,0,4096);
	
	int r = read(fd,buf,4096-16);
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
	TRACE("%s::Scanf(%s)\n",getClassName(),getName());

	if (buf == NULL) {
		EPRINTF("buffer is NULL on %s/%s::Scanf()\n",getName(),getClassName());
		return false;
	}		
	int sl = strlen(buf);
	if (sl < 3) {
		EPRINTF("buffer too short on %s/%s::Scanf()\n",getName(),getClassName());
		return false;
	}
	char *c_end = &buf[sl];
	char fmt[1024];

	TRACE("processing %s/%s\n",getName(),getClassName());

	if (Sptr == NULL) {
		EPRINTF("Sptr is NULL\n");
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
		fprintf(stderr,"t_Range::Update(%g) %g<x<%g\t",d,min,max); 
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
	TRACE("%s %s::t_NamedRange::Scanf(%s)\n",getName(),getClassName(),st);

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
void t_laverage::Add(double v)
{
	delta = (v - AvgV);
	gradient = (delta / alen);
	AvgV += gradient;
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
		sprintf(buf,"{avgv:%g, len:%g, min:%g, max:%g}%s",AvgV,alen,min,max,getName());
	}		
	return buf;
}

bool t_NamedAverage::Scanf(char *st)
{
	if (st == NULL) {
		return false;
	}
	TRACE("%s %s::t_NamedAverage::Scanf(%s)\n",getName(),getClassName(),st);

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
		AvgV = v;		// directly set average
	} else {
		t_NamedAverage::Add(v);	// calculate average, update range
	}
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//		t_average
//
//_______________________________________________
void t_average::Add(double v)
{
	delta = (v - AvgV);
	gradient = (delta / alen);

	double qd = delta*delta;			// quadratic error

	if (updateCnt < windowLength) {
		windowSum += v;
		AvgV = windowSum / (updateCnt+1);	// correct value for the very first steps

		qErr.alen = (updateCnt+1.0);
		qErr.Add(qd);
	} else {
		qErr.Add(qd);
		qErr.Update(qErr.AvgV);

		AvgV += gradient;
	}


	if (calibration) {
	    Update(AvgV);	// expand ranges slowly
	} else {
	    Update(v);		// quickly expand ranges
	}    

	updateCnt++;
}

void t_average::setCalibrationLen(double l)
{
	alen = l;
	calibration = true;
	Reset();

	qErr.Reset();
	qErr.alen = l;
}

void t_average::finishCalibration(void) 
{
	alen = windowLength;
	calibration = false;
	freezeBoundaries = true;	
}

void t_average::Init(double l)
{
	alen = l;
	windowLength = alen;
	windowSum = 0.0;
	updateCnt = 0;

	qErr.alen = alen;
	qErr.Reset();

//	AvgV = 0.0;
	t_laverage::Reset();
}

double t_average::Filtered(double fs)
{
    return t_Range::Scaled11(AvgV) * fs;
}


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//		t_window_stack
//
//_______________________________________________
void t_window_stack::Init(int l)
{
	len = l;
	idx = 0;
	cnt = 0;

	V = new double [len];
	
	Reset();
}


void t_window_stack::Reset(void)
{
	ZeroIt();
	t_average::Reset();
}

void t_window_stack::ZeroIt(void)
{
	for (int i=0; i<len; i++)
	   V[i] = 0.0;	
}


void t_window_stack::Done(void)
{
//	fprintf(stderr,"this:0x%08lx\tV:0x%08lx\n",(long)this,(long)V);

	if (V != NULL)
	   delete V;

	V = NULL;
	len = 0;
}


void t_window_stack::Add(double v)
{
	V[idx] = v;

	if (cnt < len)
	   cnt++; 

	idx++;
	idx %= len;

	t_average::Add(v);
}


double t_window_stack::Get(int i)
{
//	if ((i >= len) || (i >= cnt))
//	   {
//	    return 0.0;
//	   }
	    
	int j = idx + len - 1 - i;
        j %= len;

        return V[j]; 
}

double t_window_stack::Get(double I)
{
	fprintf(stderr,"t_window_stack::Get(double %g)\n",I);

	double f = floor(I);
	double c = ceil(I);

	int v = (int)f;
	int w = (int)c;

	if ((w-v) == 0)
	   fprintf(stderr,"ceil == floor\n");

	double a = Get(v);
	double b = Get(w);

	double d = I - f;
	double e = (b - a) * d;

	return (a + e);
}

double t_window_stack::Value(void)
{
	double S = 0.0;
	for (int i=0; i<len; i++)
	  S += V[i];

        return S/(double)cnt;
}


double t_window_stack::Info(void)
{
	double S = 0.0;
	for (int i=0; i<len; i++)
	  S += (V[i] * log(V[i]));


	double	d = -S / log((double)len);
	return d;
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
