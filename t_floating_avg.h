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

#include "tns_util/daemonic.h"


//_________________________________________________________
//
class t_NamedObj {
private:
	t_NamedObj **Sptr;
	char ItsName[64];
public:
	t_NamedObj(); 
	
	void SetSubPointer(t_NamedObj **);
	
	char *getName(void) { return ItsName; };
	void SetName(char *n); 
	
	virtual bool Save(int fd);
	virtual bool Load(int fd);
	
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
public:
	t_Range() {
		changed = 0;
		last = 1.0;
	};
	double max,min,size;
	void Reset(void);
	bool Update(double d);
	bool HasChanged(void);
	double scaled(void);
	short scaled2(void);
	int Changed(void) { return changed; };
	double Last(void) { return last; };
};


//_________________________________________________________
//
class t_NamedRange : public t_Range, public t_NamedObj {
public:	
	virtual const char *getClassName(void) { return "t_NamedRange"; };
	virtual char *Printf(char *buf);
	virtual bool Scanf(char *buf);	
};


//_________________________________________________________
//
class t_laverage : public t_Range {
public:
	double AvgV,alen;

	t_laverage() {
		AvgV = 1.0;
		alen = 1.0;
	};
	double Range(void);

	virtual void Add(double);	
	virtual void Reset(void);
};


//_________________________________________________________
//
class t_average : public t_laverage {
public:
	void Init(double);
	void Init(t_laverage *p) {
		if (p != NULL) {
			AvgV = p->AvgV;
			alen = p->alen;
		}
	};
	virtual void Add(double v);	
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
	virtual const char *getClassName(void) { return "t_NamedSensorAverage"; };
	virtual void Add(double v, bool bWarmup);	
};

//_________________________________________________________
//
class t_window_stack : public t_average {
public:
	int len,idx,cnt;
	double *V;

	void Init(int length);
	void Done(void);
	void ZeroIt(void);		

	double Get(int i);
	double Get(double I);
	double Value(void);		// return average

	double Info(void);		// shanon
	virtual void Add(double v);		// take the next value into the window
	virtual void Reset(void);
					// and remove the last value
        
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
