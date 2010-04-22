/* 
 * Thomas Pantzer: t_Set.cc, v0.1b  16.1.2001 3:27 
 * 
 * 
 * Copyright 2001 by Thomas Pantzer
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
 * thomas@pantzer.net   (Thomas Pantzer) 
 * 
 */



#define _T_SET_G
#include "tns_util/t_Set.h"
#include "tns_util/utils.h"

#include <stdio.h>
#include <string.h>

//#define SHOW_COMPILER_MOD 
#include "tns_util/copyright.h"


//module definition and procedures go here

void t_Set::Init(unsigned int n)
{
      Flags = new unsigned char [(n / 8) +2];
      size = n;	 
}

void t_Set::Increase(unsigned int p)
{
	unsigned char *of = Flags;
	int os = size;
	
	Flags = new unsigned char [((p+1) / 8) +2];
	int k = (os / 8) +1;
	memcpy(Flags,of,k);

	delete of;
	
	size = p+1;
	
	for (unsigned int i=os; i<=p; i++)	{
		Del(i);
	}
}

void t_Set::Add(unsigned int p)
{
   if (Flags == NULL)
      return;

//	if (p/8 >= size/8) {
//		Increase(p);
//	}
   if (p >= size)
      return; 
      
   int hi = p / 8;   
   int lo = p % 8; 

   unsigned char mask = 1 << lo;

	if ((Flags[hi] & mask) == 0) {
		Flags[hi] |= mask;
		     
		elements++;
	}		
}

void t_Set::Del(unsigned int p)
{
   if (Flags == NULL)
      return;

   if (p >= size)
      return;
            
   int hi = p / 8;   
   int lo = p % 8; 

	unsigned char mask = 1 << lo;
   
	if ((Flags[hi] & mask) > 0) {

		mask = 255 - mask;
		Flags[hi] &= mask;

		elements--;
	}
}

bool t_Set::Contains(unsigned int p)
{
   if (Flags == NULL)
      return false;

   if (p >= size)
      return false; 
      
   int hi = p / 8;   
   int lo = p % 8; 

   unsigned char mask = 1 << lo;
   
   if ((Flags[hi] & mask) > 0)
      return true;
    else
      return false;  
}


int t_Set::Value(unsigned int n)		
{
	unsigned int j = 0;
   
	for (unsigned int i=0; i<size; i++) {
		if (Contains(i)) {
			if (j == n) {
				return i;
				// the n-th set element is has value i 
			}				
			j++;
		}				
	}   
	return -1;      
}	 	 


// returns n if k is the n-th set bit in set
int t_Set::Position(unsigned int k)
{
	unsigned int j = 0;
   
	for (unsigned int i=0; i<size; i++) {
		if (Contains(i)) {
			if (i == k) {
				return j;	
				// element with value k is ordered at position j
			}				
			j++;
		}				
	}   
	return -1;      
}	 	 

// returns the count of set bits in set
int t_Set::Members(void)
{
   return elements;
}	 	 

void t_Set::Fill(void)
{
   for (unsigned int i=0; i<size; i++)
     Add(i);
     
    elements = size;
}

void t_Set::Clear(void)
{
   for (unsigned int i=0; i<size; i++)
     Del(i);
     
    elements = 0;
}

void t_Set::Invert(void)
{
   for (unsigned int i=0; i<size; i++)
     Toggle(i);
}


void t_Set::Or(t_Set *s)
{
	if (s == NULL)
		return;
	
	for (unsigned int i=0; i<(s->size); i++) {
		if (s->Contains(i)) {
			Add(i);
		}
	}
}

void t_Set::And(t_Set *s)
{
	if (s == NULL)
		return;
	
	int k = __max(size,s->size);
	
	for (int i=0; i<k; i++) {
		if (!s->Contains(i) || !Contains(i)) {
			Del(i);
		}
	}
}


void t_Set::Print(void)
{
	char st[4096];		// size < 4096 ???
	printf("%s",Print(st));
}

char *t_Set::Print(char *st)
{
//	st[0] = '\0';
	unsigned int k = 0;
	for (k=0; k<size; k++) {
		int i = k / 8;
		int j = k % 8;
		char m = 1 << j;
        
		if ((Flags[i] & m) > 0) {
			st[k] = '1';
		} else {
			st[k] = '_';  
		}			
	}  
	st[k] = '\0';
	fprintf(stderr,"%d/%d, %s\n",strlen(st),size,st);
	return st;
}







