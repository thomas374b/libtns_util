/*
 * t_protected_avg.h
 *
 *  Created on: 17.10.2013
 *      Author: pantec
 */

#ifndef T_PROTECTED_AVG_H_
#define T_PROTECTED_AVG_H_

#include "tns_util/t_floating_avg.h"

class t_protected_avg : public t_average {
protected:
	t_laverage qErr;
public:
	t_protected_avg() {
		qErr.Init(32.0);
	};

	virtual ~t_protected_avg() {};

	virtual void Add(double) {
		updateGradient();
		double qd = delta*delta;			// quadratic error
		qErr.Add(qd);
	};
};


#endif /* T_PROTECTED_AVG_H_ */
