/*
 * tlongaverage.cpp
 *
 *  Created on: 25.05.2018
 *      Author: pantec
 *
 *
 *  implementation of an EWMA for data-type "long"
 *  EWMA = exponential weighted moving average
 *
 */

#include "t_long_average.h"

#include <stdio.h>
#include <string.h>

t_long_average::t_long_average(int len)
{
	fprintf(stderr, "constructor::t_long_average(%d)\n", len);
	short m = 1;
	pow = 0;
	while(m < len) {
		m <<= 1;
		pow++;
	}
	length = m;
	mod = length -1;
	_data = new long[m+4];

	memset(_data, 0, sizeof(long) * (m+2)  );
/*
	for (int i=0; i<len; i++) {
		data[i] = 0;
	}
*/
	idx = 0;
	cnt = 0;
	sum = 0;
//	gradSum = 0;

//	fprintf(stderr,"len:%d  mod:0x%02x  pow:%d\n", length, mod, pow );
}

t_long_average::~t_long_average()
{
	fprintf(stderr, "destructor::t_long_average(): %ld\n", (long)_data);
	if (_data != NULL) {
		delete _data;
		_data = NULL;
	}
}

long t_long_average::last()
{
	return _data[idx];
}



long t_long_average::center()
{
	short idx_1 = (idx + (length >> 1)) & mod;
	return _data[idx_1];
}


void t_long_average::add(long v)
{
	short idx_1 = (idx + mod) & mod;

	int dp = 0;
	if (cnt > 0) {
		dp = v - _data[idx_1];
	}

	gradient = v - expAvg;
	expAvg += (gradient / length);

//	gradSum += dp;

	int dm = 0;
	if (cnt >= length) {
		dm = _data[idx] - _data[idx_1];
	}
//	gradSum -= dm;

	sum -= _data[idx];		// alter wert raus
	_data[idx] = v;
	sum += _data[idx];		// neuer rein

//	adjustMaxMin(v);

	idx++;
	idx &= mod;

	if (cnt < length) {
		cnt++;
	}
}

long t_long_average::value()
{
	if (cnt < length) {
		if (cnt < 1) {
//			fprintf(stderr, "t_int_average::value() division by zero\n");
			return 0;
		}
		return sum / cnt;
	}
	return sum >> pow;
}

long t_long_average::avg()
{
	return expAvg;
}


