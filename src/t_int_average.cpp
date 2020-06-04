/*
 * tintaverage.cpp
 *
 *  Created on: 28.08.2015
 *      Author: pantec
 */

#include "t_int_average.h"

#include <stdio.h>
#include <math.h>

t_int_average::t_int_average(int len)
{
	short m = 1;
	pow = 0;
	while(m < len) {
		m <<= 1;
		pow++;
	}
	length = m;
	mod = length -1;
	data = new int[m];
	for (int i=0; i<len; i++) {
		data[i] = 0;
	}
	idx = 0;
	cnt = 0;
	minMaxReset();
	sum = 0;
	gradSum = 0;

	lastGradient = 0;
	atTop = false;
	atBottom = false;

	fprintf(stderr,"len:%d  mod:0x%02x  pow:%d\n", length, mod, pow );
}

void t_int_average::minMaxReset()
{
	min =  2147483647;
	max = -2147483647;
}

int t_int_average::trackGradient(int g)
{
	if ((lastGradient < 0) && (g > 0)) {
		atBottom = true;
	}
	if ((lastGradient > 0) && (g < 0)) {
		atTop = true;
	}
	lastGradient = g;
	return g;
}


bool t_int_average::isAtBottom()
{
	if (atBottom) {
		atBottom = false;
		return true;
	}
	return false;
}

bool t_int_average::isAtTop()
{
	if (atTop) {
		atTop = false;
		return true;
	}
	return false;
}



void t_int_average::adjustMaxMin(int v)
{
	if (v > max) {
		max = v;
	}
	if (v < min) {
		min = v;
	}
}

void t_int_average::add(int v)
{
/*
	short idx_1 = (idx + mod) & mod;

	int dp = 0;
	if (cnt > 0) {
		dp = v - data[idx_1];
	}
	gradSum += dp;

	int dm = 0;
	if (cnt >= length) {
		dm = data[idx] - data[idx_1];
	}
	gradSum -= dm;
*/
	gradSum = v - data[idx];
	sum += gradSum;

//	sum -= data[idx];		// alter wert raus
	data[idx] = v;
//	sum += data[idx];		// neuer rein

	adjustMaxMin(v);

	idx++;
	idx &= mod;

	if (cnt < length) {
		cnt++;
	}
}

int t_int_average::gradient()
{
	if (cnt < length) {
		if (cnt < 1) {
			fprintf(stderr, "t_int_average::gradient() division by zero\n");
			return 0;
		}
		return trackGradient(gradSum / cnt);
	}
	return trackGradient(gradSum >> pow);
}

int t_int_average::value()
{
	if (cnt < length) {
		if (cnt < 1) {
			fprintf(stderr, "t_int_average::value() division by zero\n");
			return 0;
		}
		return sum / cnt;
	}
	return sum >> pow;
}

int t_int_average::range()
{
	return max -min;
}

/**
 * remove highest and lowest value from average calculation
 */
int t_int_average::cooked()
{
	int p = -2147483647;
	int q = 2147483647;
	minMaxReset();
	unsigned short qi = 0;
	unsigned short pi = 0;

	for (unsigned short i=0; i<cnt; i++) {
		if (data[i] > p) {
			p = data[i];
			pi = i;
		}
		if (data[i] < q) {
			q = data[i];
			qi = i;
		}
	}
	long S = 0;
	unsigned short k = 0;
	for (unsigned short i=0; i<cnt; i++) {
		if (i == qi) {
			continue;
		}
		if (i == pi) {
			continue;
		}
		adjustMaxMin(data[i]);
		S += data[i];
		k++;
	}
	if (k < 1) {
		fprintf(stderr, "t_int_average::cooked() division by zero\n");
		return 0;
	}
	return S / k;
}

double t_int_average::Filtered(double scale)
{
	int R = range();

	if (R == 0) {
		fprintf(stderr, "t_int_average::Filtered() division by zero\n");
		return 0.0;
	}

	int C = cooked();

	if (min >= 0) {
		// range 0 .. 1
		return (C - min) / R;
	}
	// range -1 .. 1
	int bias = (max + min) / 2;

	return ((C - bias) * 2) / R;
}

char *t_int_average::Printf(char *buf)
{
	buf[0] = '\0';
	sprintf(buf, "%d", value());
	return buf;
}

t_int_average::~t_int_average()
{
	delete data;
}


/**
 * return  information according to Shannon
 */
double t_int_average::Info(void)
{
	double S = 0.0;

	for (int i=0; i<cnt; i++) {
		if (data[i] > 0) {
			S += (data[i] * log((double)data[i]));
		}
	}

	double d = -S / log((double)length);
	return d;
}

