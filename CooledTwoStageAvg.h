/*
 * CooledTwoStageAvg.h
 *
 *  Created on: 30.12.2018
 *      Author: pantec
 *
 *
 *  implementation of a windowed average with equally weighted elements over the past n steps
 *  the largest and smallest values are not taken into account
 *
 */

#ifndef COOLEDTWOSTAGEAVG_H_
#define COOLEDTWOSTAGEAVG_H_

#include "tns_util/CooledAverage.h"

#ifndef MODNAME
	#define MODNAME __FILE__
	#include "tns_util/copyright.h"
#endif

class CooledTwoStageAvg : public CooledAverage {
private:
	CooledAverage *stack;
	CooledAverage *derivate;
public:

	void Init(int len, bool ignoreMinMax=true);
	CooledTwoStageAvg();

	CooledTwoStageAvg(int len, bool ignoreMinMax=false);
	virtual ~CooledTwoStageAvg() {
		delete stack;
		delete derivate;
	};

	virtual bool add(double v);
	virtual double get();
	virtual double acceleration() { return derivate->gradient(); };
	virtual void reset();
};

#endif /* COOLEDTWOSTAGEAVG_H_ */
