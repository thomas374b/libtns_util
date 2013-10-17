/*
 * t_freezing_avg.h
 *
 *  Created on: 17.10.2013
 *      Author: pantec
 */

#ifndef T_FREEZING_AVG_H_
#define T_FREEZING_AVG_H_


class t_freezing_avg {
protected:
	int cnt;
	double Sum;
public:
	t_freezing_avg() {
		Init();
	};
	virtual ~t_freezing_avg() {};


	void Init() {
		cnt = 0;
		Sum = 0.0;
	};

	void Add(double d) {
		cnt++;
		Sum += d;
	}

	double Value() {
		if (cnt > 0) {
			return Sum / (double)cnt;
		}
		return 0.0;
	}
};

#endif /* T_FREEZING_AVG_H_ */
