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
	int lastGradient;
	bool atTop, atBottom;

	int trackGradient(int g);

protected:
public:
	t_int_average(int len);
	virtual ~t_int_average();

	void add(int);
	int gradient();
	int value();
	int range();
	int cooked();

	double Filtered(double scale);

	bool isAtTop();
	bool isAtBottom();

	char *Printf(char *b);

	void adjustMaxMin(int v);
	void minMaxReset();

	double Info(void);
};

#endif /* TINTAVERAGE_H_ */
