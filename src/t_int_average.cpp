/*
 * tintaverage.cpp
 *
 *  Created on: 28.08.2015
 *      Author: pantec
 */

#include "t_int_average.h"

#include <stdio.h>


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
	min =  1000000000;
	max = -1000000000;
	sum = 0;
	gradSum = 0;

	fprintf(stderr,"len:%d  mod:0x%02x  pow:%d\n", length, mod, pow );
}

void t_int_average::add(int v)
{
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

	sum -= data[idx];		// alter wert raus
	data[idx] = v;
	sum += data[idx];		// neuer rein

	if (v > max) {
		max = v;
	}
	if (v < min) {
		min = v;
	}

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
		return gradSum / cnt;
	}
	return gradSum >> pow;
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

int t_int_average::cooked()
{
	int p = -1000000000;
	int q =  1000000000;
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
		return 0;
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

