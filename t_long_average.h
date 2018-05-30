/*
 * tlongaverage.h
 *
 *  Created on: 25.05.2018
 *      Author: pantec
 */

#ifndef T_LONG_AVERAGE_H_
#define T_LONG_AVERAGE_H_


class t_long_average {
//	int len;

	unsigned short pow;
//	int min;
//	int max;
	long sum;
	long gradient;
	long expAvg;

protected:
	unsigned short length;
	unsigned short idx;
	unsigned short cnt;
	unsigned short mod;
public:
	long *_data;

	t_long_average(int len);
	virtual ~t_long_average();

	void add(long val);
	long value();
	long avg();
	long last();
	long center();
};


#endif /* T_LONG_AVERAGE_H_ */
