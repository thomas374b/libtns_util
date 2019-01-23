/*
 * CoolTwoStageNamedAvg.h
 *
 *  Created on: 30.12.2018
 *      Author: pantec
 */

#ifndef COOLTWOSTAGENAMEDAVG_H_
#define COOLTWOSTAGENAMEDAVG_H_

#include "tns_util/CooledTwoStageAvg.h"
#include "tns_util/t_floating_avg.h"



class CoolTwoStageNamedAvg : public CooledTwoStageAvg, public t_NamedObj {
public:
	CoolTwoStageNamedAvg(int len, bool ignoreMinMax=false);

	virtual const char *getClassName(void) { return "CoolTwoStageNamedAvg"; };
	virtual char *Printf(char *buf);
	virtual bool Scanf(char *buf);
};

#endif /* COOLTWOSTAGENAMEDAVG_H_ */
