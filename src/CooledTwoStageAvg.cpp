/*
 * CooledTwoStageAvg.cpp
 *
 *  Created on: 30.12.2018
 *      Author: pantec
 */

#include "CooledTwoStageAvg.h"

void CooledTwoStageAvg::Init(int len, bool ignoreMinMax)
{
	CooledAverage::Init(len, ignoreMinMax);
	stack->Init(len, ignoreMinMax);
	derivate->Init(len, ignoreMinMax);
}

CooledTwoStageAvg::CooledTwoStageAvg()
{
	Init(4, true);
}


CooledTwoStageAvg::CooledTwoStageAvg(int len, bool noMinMax)
: CooledAverage(len, noMinMax)
{
	stack = new CooledAverage(len, noMinMax);
	derivate = new CooledAverage(len, noMinMax);
}

double CooledTwoStageAvg::get()
{
	if (filled == 0) {
		return stack->get();
	}
	return CooledAverage::get();
}

bool CooledTwoStageAvg::add(double v)
{
	bool result = false;

	if (stack->add(v)) {
		result = CooledAverage::add( stack->get() );
	}
	derivate->add( stack->gradient() );

	return result;
}

void CooledTwoStageAvg::reset()
{
	CooledAverage::reset();
	stack->reset();
	derivate->reset();
}


