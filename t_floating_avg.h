/* 
 * Thomas Pantzer: t_floating_avg.h, v0.1b  8.6.99 16:49 
 * 
 * 
 * Copyright 1999 by Thomas Pantzer
 * 
 * Permission to use, copy, modify, and distribute this software for noncommercial 
 * use and without fee is hereby granted, provided that the above copyright notice 
 * appear in all copies and that both that copyright notice and this permission 
 * notice appear in supporting documentation, and that the name of the Author not be 
 * used in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. The Author makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is" 
 * without expressed or implied warranty. 
 * 
 * 
 * pantec@isys2.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */


#ifndef T_FLOATING_AVG_H
#define T_FLOATING_AVG_H


#include "tns_util/copyright.h"
#include "tns_util/porting.h"
#include "tns_util/daemonic.h"
#include "tns_util/t_freezing_avg.h"

extern char *shortFract(double d, char *buf);
extern double fraction(double d);

//_________________________________________________________
//
class t_NamedObj {
private:
	t_NamedObj **Sptr;
	char ItsName[64];
public:
	t_NamedObj(); 
	virtual ~t_NamedObj() {
	};
	
	void SetSubPointer(t_NamedObj **);
	char *getName(void) { return ItsName; };

	virtual void SetName(const char *n);
	
	virtual bool Save(fileHandle fd);
	virtual bool Load(fileHandle fd);
	
	virtual char *Printf(char *buf) { return buf; };
	virtual bool Scanf(char *buf);

	virtual const char *getClassName(void) = 0;
};



//_________________________________________________________
//
class t_Range {
private:
	int changed;
	double last;
protected:
	bool freezeBoundaries;	
public:	
	double bias;
	double fullScale;

	t_Range() {
		changed = 0;
		last = 1.0;
		bias = 0.0;
		freezeBoundaries = false;
		_max = -100000000000000.0;
		_min =  100000000000000.0;
		size = 0.0;
		fullScale = 1.0;
	};
	double _max, _min, size;
	void Reset(void);
	bool Update(double d);
	bool HasChanged(void);
	double scaled(void);
	short scaled2(void);
	int Changed(void) { return changed; };
	double Last(void) { return last; };
	double Scaled11(double x);
	void setBias(double);
	void setFullScale(double);
};


//_________________________________________________________
//
class t_NamedRange : public t_Range, public t_NamedObj {
public:	
	virtual ~t_NamedRange() {
	};
	virtual const char *getClassName(void) { return "t_NamedRange"; };
	virtual char *Printf(char *buf);
	virtual bool Scanf(char *buf);	
};


//_________________________________________________________
//
class t_laverage : public t_Range {
public:
	double AvgV,alen,gradient,delta;

	t_laverage() {
		AvgV = 1.0;
		alen = 1.0;
		gradient = 0.0;
		delta = 0.0;
	};

	t_laverage(int L) {
		AvgV = 1.0;
		alen = L;
		gradient = 0.0;
		delta = 0.0;
	};

	virtual ~t_laverage() {
	};

	double updateGradient(double v);
	double Range(void);

	virtual void Add(double);	
	virtual void Reset(void);
};

//_________________________________________________________
//
class t_average : public t_laverage, t_freezing_avg {
private:
	double windowLength;	// TODO: review against "alen" member
	bool autoCalibration;
public:

	t_average() {
	    autoCalibration = false;
	    windowLength = 1.0;
	};

	void Init(double);

	void Init(t_laverage *p) {
		if (p != NULL) {
			AvgV = p->AvgV;
			alen = p->alen;
			windowLength = alen;
		}
	};

	bool _frozen(void);	// return true if more values than window-length values have been already added

	virtual void Add(double v);	
	double Filtered(double fullScale);
	void calibrate(double min, double max);
	void setCalibrationLen(double windowLength);
	void finishCalibration(void);
};


//_________________________________________________________
//
class t_NamedAverage : public t_average, public t_NamedObj {
public:	
	virtual const char *getClassName(void) { return "t_NamedAverage"; };
	virtual char *Printf(char *buf);
	virtual bool Scanf(char *buf);
};

//_________________________________________________________
//
class t_NamedSensorAverage : public t_NamedAverage {
public:	
	t_NamedSensorAverage(int len) : t_NamedAverage() {
		Init(len);
	};
	virtual const char *getClassName(void) { return "t_NamedSensorAverage"; };
	//	virtual void Add(double v, bool bWarmup);
	//	we dont need this warmup stuff,  anymore
	virtual bool add(double v);
	double get() { return AvgV; };
};



#endif







/**/
/**/
/**/
/**/
/**/
/**/
/**/
/**/
/*****  End Of File  ****************************/
