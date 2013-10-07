/* 
 * Thomas Pantzer: t_sorter.cc, v0.1b  24.11.98 10:32 
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

// Output from p2c 1.21alpha-07.Dec.93, the Pascal-to-C translator
// From input file "/home/pantec/src/functions/t_sorter.p"
// #include <p2c/p2c.h>


#define T_SORTER_G
#include "tns_util/t_sorter.h"
#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"




#ifdef DEBUG
#include <stdio.h>
#endif


void t_sorter::Init(void **p, t_realfunc func, int s, bool a)
{
  A = p;
  valfunc = func;
  size = s;
  ascending = a;

  T0 = new void*[size];
  T1 = new void*[size];
}

void t_sorter::Done()
{
  delete [] T0;
  delete [] T1;
}


void *t_sorter::MaxElem(void)
{
  double max,d;
  int mi;
  
  max = 0.0;
  mi = 0;
  
  for (int i=0; i<size; i++)
    {
     d = value(i);
     
     if (d > max)
        {
         max = d;
         mi = i;
        }
    }
  
  return A[mi];
}

void *t_sorter::MinElem(void)
{
  double min,d;
  int mi;
  
  min = Max();
  mi = 0;
  
  for (int i=0; i<size; i++)
    {
     d = value(i);
     
     if (d < min)
        {
         min = d;
         mi = i;
        } 
    }
  
  return A[mi];
}

double t_sorter::Max(void)
{
  double d;
  
  max = 0.0;
  
  for (int i=0; i<size; i++)
    {
     d = value(i);
     
     if (d > max)
         max = d;
    }
  
  return max;
}

double t_sorter::Min(void)
{
  double d;
  
  min = max;
  
  for (int i=0; i<size; i++)
    {
     d = value(i);
     
     if (d < min)
         min = d;
    }
  
  return min;
}

double t_sorter::value(int i)
{
  double d;
#ifdef DEBUG  
  fprintf(stderr,"t_sorter->value(%d)=",i);
#endif  
  d = valfunc(A[i]);
#ifdef DEBUG  
  fprintf(stderr,"%g\n",d);
#endif  

  if (ascending == true)
      return d;
   else
      return -d; 
}


char t_sorter::higher(int i, double d)
{
#ifdef DEBUG
  fprintf(stderr,"t_sorter->higher(%d,%g)\n",i,d); 
#endif
  
  if (value(i) > d)
    return true;
  else
    return false;
}


double t_sorter::Average(void)
{
   return (Average(0,size-1));
}

double t_sorter::Average(int start, int ende)	// sub interval average 
{
  int k = 0;
  double d = 0.0;

#ifdef DEBUG  
  fprintf(stderr,"t_sorter->Average(%d,%d)\n",start,ende);
#endif  

  int FORLIM_;
  
  if (ende < size)
     FORLIM_ = ende;
    else
     FORLIM_ = size -1; 

  for (int i = start; i <= FORLIM_; i++)
    {
     d += value(i);
     k++;
    }

  return (d / k);
}


#ifdef DEBUG
int reccount = 0;
#endif

void t_sorter::sort(int start, int ende)  // recursive sort procedure
{
	int l = 0;
	int h = 0;

#ifdef DEBUG
	fprintf(stderr,"%d. t_sorter->sort(%d,%d)\n",reccount,start,ende);
	reccount++;
#endif
	int FORLIM_;
  
	if (ende < size) {
		FORLIM_ = ende;
	} else {
		FORLIM_ = size -1; 
	}
	if (start >= FORLIM_) {
		return;			// don't sort lists with only one element
	}
	if ((start+1) == FORLIM_) {
		// case for lists with 2 elements 
                               
		if (value(start) > value(FORLIM_)) { 
			// swap if not correct order
			void *p;
          
			p = A[start];
			A[start] = A[FORLIM_];
			A[FORLIM_] = p;
		}
		return;   
	}     

	double avg = Average(start, FORLIM_);

	for (int i = start; i <= FORLIM_; i++) {
		if (higher(i, avg)) {
			T1[h] = A[i];
			h++;
		} else {
			T0[l] = A[i];
			l++;
		}
	}
  
	if ((h == 0) || (l == 0))	{	// all elements in list have same value 
		return;                            // and other list is empty
	}

	{
		for (int i = 0; i < l; i++) {
			A[i + start] = T0[i];
		}
	}
	{
		for (int i = 0; i < h; i++) {
			A[i + l + start] = T1[i];
		}
	}
#ifdef DEBUG
  fprintf(stderr,"%d. lolist=%d\n",reccount,l);
#endif

  sort(start, start + l - 1);

#ifdef DEBUG
  fprintf(stderr,"%d. hilist=%d\n",reccount,h);
#endif
  
  sort(start + l, ende);

#ifdef DEBUG  
  reccount--;
  fprintf(stderr,"%d. ready\n",reccount);
#endif  
}

void t_sorter::Run(void)
{
	Run(0);
}

void t_sorter::Run(int start)
{
  max = Max();
  min = Min();
  sort(start, size - 1);
}





// End.
