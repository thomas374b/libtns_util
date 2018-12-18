/*
 * t_window_stack.cpp
 *
 *  Created on: 17.10.2013
 *      Author: pantec
 */


#include <math.h>

#include "tns_util/t_window_stack.h"
#include "tns_util/daemonic.h"


void t_window_stack::Init(int l)
{
	len = l;
	idx = 0;
	cnt = 0;
	Sum = 0.0;

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
	for (int i=0; i<len; i++) {
		V[i] = 0.0;
	}
}



void t_window_stack::Add(double v)
{
	Sum -= V[idx];		// subtract last value
	V[idx] = v;
	Sum += V[idx];		// add new value

	if (cnt < len) {
	   cnt++;
	}

	idx++;
	idx %= len;

	t_average::Add(v);
}


double t_window_stack::Get(int i)
{
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

	if ((w-v) == 0) {
	   EPRINTF("ceil == floor");
	}

	double a = Get(v);
	double b = Get(w);

	double d = I - f;
	double e = (b - a) * d;

	return (a + e);
}

double t_window_stack::Value(void)
{
	if (cnt == 0) {
		return 0.0;
	}
	return Sum / (double)cnt;
}

/**
 * return  information according to Shannon
 */
double t_window_stack::Info(void)
{
	double S = 0.0;
	for (int i=0; i<len; i++) {
		S += (V[i] * log(V[i]));
	}

	double d = -S / log((double)len);
	return d;
}

