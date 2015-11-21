/* 
 * Thomas Pantzer: astropak.h, Version: v0.1a  22.04.2010 13:46 
 * 
 * 
 * Copyright 2010 by Thomas Pantzer
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
 * pantec@doha.private.site   (Thomas Pantzer,,,) 
 * 
 */

#ifndef _ASTROPAK_H
#define _ASTROPAK_H

typedef struct {
	int hour, minute, second;
} t_timestruct;


#include "tns_util/win32_export.h"

#if _WINDOWS | WIN32
#else
	#include <sys/time.h>
#endif

TNS_UTIL_API double sunRise(double height, time_t now);
TNS_UTIL_API double sunSet(double height, time_t now);
TNS_UTIL_API bool atDayTime(double height, time_t now);
TNS_UTIL_API t_timestruct *getSunRise(void);
TNS_UTIL_API t_timestruct *getSunSet(void);
TNS_UTIL_API double Zeitgleichung(double T);
TNS_UTIL_API double halberTag(double T, double hight, double Breite);
TNS_UTIL_API double Deklination(double T);


TNS_UTIL_API double AP_power(double x, int n);
TNS_UTIL_API double AP_fak(int x);
TNS_UTIL_API double AP_sin(double x, int precision);
TNS_UTIL_API double AP_cos(double x, int precision);
TNS_UTIL_API double AP_arccos(double x, int precision);

extern int _tnsutil_daylightSaving;

#define sunStartStop			0.0
#define buergerDaemmerung		-6.0
#define nauticDaemmerung		-12.0
#define astroDaemmerung			-18.0

#include <math.h>

#define deg2rad(x)	((x*M_PI)/180.0)
#define rad2deg(x)	((x*180.0)/M_PI)
#define SQR(x)		fabs(fabs(x) * fabs(x))
#define frac(x)		((x)-floor(x))

TNS_UTIL_API_CLASS t_coor {
public:
	double ra;
	double dec;
	double lat;
	double lon;

	double az;
	double alt;
};

TNS_UTIL_API_CLASS t_SunPosition : public t_coor {
private:
	double	anomalyMean;
	double	distance;
	double 	diameter;
	double	parallax;
//	double	sign;	

	// Transform ecliptical coordinates (lon/lat) to equatorial coordinates (RA/dec)
	void Ecl2Equ(double TDT);

	// Transform equatorial coordinates (RA/Dec) to horizonal coordinates (azimuth/altitude)
	// Refraction is ignored
	void Equ2Altaz(double TDT, double geolat, double lmst);
	
	// Calculate coordinates for Sun
	// Coordinates are accurate to about 10s (right ascension) 
	// and a few minutes of arc (declination)
	void Calculate(double TDT, double geolat, double lmst);

public:
	double Altitude(double lat, double lon, struct tm *ltime, int TZ);
};


extern double geographische_Laenge;
extern double geographische_Breite;


#endif
