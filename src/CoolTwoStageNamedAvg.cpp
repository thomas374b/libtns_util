/*
 * CoolTwoStageNamedAvg.cpp
 *
 *  Created on: 30.12.2018
 *      Author: pantec
 */

#include "CoolTwoStageNamedAvg.h"

CoolTwoStageNamedAvg::CoolTwoStageNamedAvg(int len, bool ignoreMinMax)
: CooledTwoStageAvg(len, ignoreMinMax)
, t_NamedObj()
{
}

char *CoolTwoStageNamedAvg::Printf(char *buf)
{
	if (buf != NULL) {
		char b0[32], b2[32], b3[32];
		sprintf(buf,"{avgv:%s, len:%d, min:%s, max:%s, skip:%d}%s",
				shortFract(get(), b0), length, shortFract(_min,b2), shortFract(_max,b3), skipMinMax, getName());
	}
	return buf;
}

bool CoolTwoStageNamedAvg::Scanf(char *st)
{
	if (st == NULL) {
		return false;
	}
#ifdef DEBUG
	TRACE("%s %s::Scanf(%s)\n",getName(),getClassName(),st);
#endif
	float a,i,x;
	int l,s;
	char buf[256];
	if (sscanf(st,"{avgv:%g, len:%d, min:%g, max:%g, skip:%d}%s",&a,&l,&i,&x,&s,buf) == 6) {
		if (strcmp(buf,getName()) == 0) {
			if (length != l) {
				delete data;
				data = new double[l];
				length = l;
			}
			_min = i;
			_max = x;
			skipMinMax = s;
			for (int i=0; i<length; i++) {
				data[i] = a;
			}
			idx = 0;
			return true;
		}
	}
	return false;
}

