/*
 * CooledAverage.h
 *
 *  Created on: 17.12.2017
 *      Author: pantec
 */

#ifndef COOLEDAVERAGE_H_
#define COOLEDAVERAGE_H_


typedef struct {
	bool changed;
	double val;
	double sum;
} t_double_cache;

class CooledAverage {
public:
	double *data;
	double _max, _min;
	short length, idx, maxIdx, minIdx, filled;
	bool skipMinMax;

	t_double_cache _cache;

	void Init(int len, bool ignoreMinMax=true);
	CooledAverage();
	/**
	 * len should be a power of 2
	 */
	CooledAverage(int len, bool ignoreMinMax=false);
	virtual ~CooledAverage();


	/**
	 * return true if one set/cycle is full, i.e. when # of updates modulo length == 0
	 */
	virtual bool synced();		// return true if n-adds % length == 0
	virtual bool add(double v);
	virtual double get();	// return the averaged value
	virtual double value(int idx=0);
	virtual double gradient();	// difference to the last step
	virtual double trend();		// weighted difference over all steps
	virtual void reset();
	virtual double findMinMax();

	double range();		// return max-min of the current set
	double center();	// return (max-min)/2
};

#endif /* COOLEDAVERAGE_H_ */
