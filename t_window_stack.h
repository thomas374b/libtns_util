/*
 * t_window_stack.h
 *
 *  Created on: 17.10.2013
 *      Author: pantec
 */

#ifndef T_WINDOW_STACK_H_
#define T_WINDOW_STACK_H_

#include "tns_util/t_floating_avg.h"

//_________________________________________________________
//
class t_window_stack : public t_average {
public:
	t_window_stack() {
		V = NULL;
	};
	virtual ~t_window_stack() {
		if (V != NULL) {
			delete V;
		}
		V = NULL;
		len = 0;
	};


	int len,idx,cnt;	// TODO: cnt already protected member
	double *V;
	double Sum;			// TODO: Sum already protected member

	void Init(int length);

	void ZeroIt(void);		// clears the array

	double Get(int n);		// return nth array value
	double Get(double I);	// return interpolated

	double Value(void);		// return average

	double Info(void);				// return information according to Shannon
	virtual void Add(double v);		// take the next value into the window and calculate average

	virtual void Reset(void);		// reset ranges

};



#endif /* T_WINDOW_STACK_H_ */
