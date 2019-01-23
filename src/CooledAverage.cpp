/*
 * CooledAverage.cpp
 *
 *  Created on: 17.12.2017
 *      Author: pantec
 */

#include "CooledAverage.h"

#include "tns_util/daemonic.h"

void CooledAverage::Init(int len, bool ignoreMinMax)
{
	TRACE("CooledAverage::Init(%d, %d)", len, ignoreMinMax);
//	delete data;
	data = new double[len];
	length = len;
	skipMinMax = ignoreMinMax;
	reset();
}

CooledAverage::CooledAverage()
{
	idx = 0;
	Init(4, true);
}

CooledAverage::CooledAverage(int len, bool ignoreMinMax)
{
	idx = 0;
	Init(len, ignoreMinMax);
}

void CooledAverage::reset()
{
	for (int i=0; i<length; i++) {
		data[i] = 0.0;
	}
	_cache.changed = true;
	_cache.val = 0.0;
	_cache.sum = 0.0;
	filled = 0;
}

CooledAverage::~CooledAverage()
{
	delete data;
}

double CooledAverage::range()
{
	return _max - _min;
}

double CooledAverage::center()
{
	return (_max + _min) / 2.0;
}

bool CooledAverage::synced()
{
	return (idx == 0);
}

/**
 * return true if a cycle has finished, i.e.every item got an update once
 */
bool CooledAverage::add(double v)
{
	if (filled < length) {
		filled++;
	}

	if (skipMinMax) {
		_cache.sum -= data[idx];	// alter wert raus
		_cache.sum += v;			// neuer rein
		_cache.val = _cache.sum / filled;

		// this is tracking, not skipping
		if (v >= _max) {
			_max = v;
		}
		if (v <= _min) {
			_min = v;
		}
		_cache.changed = false;
	} else {
		_cache.changed = true;	// needs recalculation in get
	}

	data[idx] = v;
	idx++;
	if (idx >= length) {
		idx = 0;
	}
	return synced();
}

/**
 * @return overall sum
 */
double CooledAverage::findMinMax()
{
	_max = -100000000000000.0;
	_min =  100000000000000.0;
	maxIdx = -1;
	minIdx = -1;

	double sum = 0.0;
	for (int i=0; i<filled; i++) {
		if (data[i] >= _max) {
			maxIdx = i;
			_max = data[i];
		}
		if (data[i] <= _min) {
			minIdx = i;
			_min = data[i];
		}
		sum += data[i];
	}

	return sum;
}

double CooledAverage::get(void)
{
	if ( ! _cache.changed ) {
		return _cache.val;
	}

	_cache.sum  = findMinMax();

	int n = filled;

	if (n == 0) {
		EPRINTF("%s: %s: prevent division by zero", __FILE__, __func__);
		return 0.0;
	}

	if (!skipMinMax) {
		if (n > 1) {
			if (minIdx != -1) {
				_cache.sum -= _min;
				n--;
			}
		}
		if (n > 1) {
			if (maxIdx != -1) {
				_cache.sum -= _max;
				n--;
			}
		}
	}
	_cache.changed = false;
	_cache.val = _cache.sum / n;
	return _cache.val;
}


/**
 * return element of n steps before
 */
double CooledAverage::value(int n)
{
	int prev = (idx -1 -n + length) % length;
	return data[prev];
}


double CooledAverage::gradient()
{
	if (filled < 2) {		// we must have at least two items to build a difference
		return 0.0;
	}
	double prev = value(1);		// this is data from one step before
	double current = value(0);	// this is the most actual data entry

	return (current - prev);
}

// weighted difference over all steps
double CooledAverage::trend()
{
	if (filled < 2) {		// we must have at least two items to build a difference
		return 0.0;
	}
	double last;
	if (filled == 8) {
		last = value(-1);	// this is the very last element that was not yet overwritten
	} else {
		last = data[0];
	}

	double current = value(0);
		// this is the most actual data entry

	return (current - last) / (length-1);
}



