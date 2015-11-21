/*
 * tintaverage.h
 *
 *  Created on: 28.08.2015
 *      Author: pantec
 */

#ifndef TINTAVERAGE_H_
#define TINTAVERAGE_H_

class t_int_average {
private:
	unsigned short length;
	unsigned short idx;
	unsigned short cnt;
	unsigned short mod;
	unsigned short pow;
	int min;
	int max;
	int *data;
	long sum;
	long gradSum;
public:
	t_int_average(int len);
	virtual ~t_int_average();

	void add(int);
	int gradient();
	int value();
	int range();
	int cooked();

	double Filtered(double scale);
	char *Printf(char *b);

};

#endif /* TINTAVERAGE_H_ */
