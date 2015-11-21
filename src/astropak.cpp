/* 
 * Thomas Pantzer: astropak.cpp, Version: v0.1a  22.04.2010 13:46 
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



#include <time.h>
#include <stdio.h>


#include "tns_util/astropak.h"
#include "tns_util/daemonic.h"

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"


double geographische_Laenge	= 12.44230;
double geographische_Breite	= 51.25549;

char buffer[256];

typedef struct {
	bool valid;
	double value;
} t_part_result;


/*
WOZ - MOZ = -0.171*sin(0.0337 * T + 0.465) - 0.1299*sin(0.01787 * T - 0.168)

Deklination = 0.4095*sin(0.016906*(T-80.086))

Zeitdifferenz = 12*arccos((sin(h) - sin(B)*sin(Deklination)) / (cos(B)*cos(Deklination)))/pi;

Aufgang Ortszeit = 12 - Zeitdifferenz - Zeitgleichung

Untergang Ortszeit = 12 + Zeitdifferenz - Zeitgleichung

Aufgang = Aufgang Ortszeit - geographische Länge /15 + Zeitzone

Untergang = Untergang Ortszeit - geographische Länge /15 + Zeitzone
*/


t_part_result zeitGleichung_T =  {false, 0.0};
t_part_result deklination_T =  {false, 0.0};
t_part_result halberTag_T =  {false, 0.0};

double lastDay = -1.0;

int _tnsutil_daylightSaving = 0;
static long _tnsutil_dayCount = 0;
static t_timestruct _tnsutil_SR, _tnsutil_SS;

/*
 *  Taylor
 *
 *          inf
 *  sin x = SUM (-1)^(n-1) * ( (   x^(2n-1)                ) /   (2n-1)!  )
 *          n=1
 *
 */
#define USE_AP_SIN_FUNC

#ifdef USE_AP_SIN_FUNC
	#define  PRECISION	22

	#define	SIN_(x)		AP_sin(x, PRECISION)
	#define	COS_(x)		AP_cos(x, PRECISION)
	#define	ACOS_(x)	AP_arccos(x, PRECISION)
	#define	ASIN_(x)	AP_arcsin(x, PRECISION)
#else
	#define	SIN_(x)		sin(x)
	#define	COS_(x)		cos(x)
	#define	ACOS_(x)	acos(x)
	#define	ASIN_(x)	asin(x)
#endif


#ifdef DBG_TRACE_ALL
	#define AP_DBG_TRACE(x...)	fprintf(x)
#else
	#define AP_DBG_TRACE(x...)
#endif




double AP_fak(int x)
{
	if (x == 0) {
		return 1.0;
	}
	if (x > 0) {
		return AP_fak(x-1)*x;
	}
	return 0.0;
}

double AP_power(double x, int n)
{
	double result = x;
	if (n == 0) {
		return 1.0;
	}
	if (n == 1) {
		return x;
	}
	if (n == 2) {
		return x*x;
	}

	int m = 1;
	while (m < n) {
		m <<= 1;
	}
	m >>= 1;

	double a = AP_power(x, m);
	double b = AP_power(x, n-m);

	return (a*b);
}

double AP_sin_sign(int n)
{
	return AP_power(-1.00000000, n-1);
}

double AP_sin_part(double x, int n)
{
	int k = (2*n) -1;

	double dividend = AP_power(x, k);
	double divisor = AP_fak(k);
	double quotient = dividend / divisor;

	return quotient;
}

double AP_sin(double x, int precision)
{
	double S = 0.0;
	int n = 1;
	while (n < precision) {
		double g = AP_sin_sign(n);
		double h = AP_sin_part(x, n);
		double d = g*h;
		S += d;
		n++;
	}
	return S;
}

double AP_cos(double x, int precision)
{
	return AP_sin(x + M_PI_2, precision);
}


double AP_fak2(int n)
{
	double faktor_odd = 1.0;
	double faktor_even = 2.0;

	double dividend = 1.0;
	double divisor = 1.0;

	for (int i=0; i<n; i++) {
		dividend *= faktor_odd;
		divisor *= faktor_even;
		faktor_odd += 2.0;
		faktor_even += 2.0;
	}
	AP_DBG_TRACE(stderr, "%f/%f * ", dividend , divisor);
	double quotient = dividend / divisor;
	return quotient;
}

double bikoeff(int n, int k)
{
	double result = AP_fak(n);
	result /= AP_fak(k);
	result /= AP_fak(n-k);
	return result;
}

double AP_arcsin(double x, int precision)
{
	double S = 0.0;
	int k = 0;
	while (k < precision) {
		double f1 = 1.0 / AP_power(2.0, 2*k);
		double f2 = bikoeff(2*k, k);
		int k21 = (2*k)+1;
		double f3 = AP_power(x, k21) / k21;

		double d = f1 * f2 * f3;
		S += d;

		k++;
	}
	return S;
}

double AP_arccos(double x, int precision)
{
	return AP_arcsin(-1.0 * x, precision) + M_PI_2;
}



void initializeAstroPartresults()
{
	AP_DBG_TRACE(stderr, "initializeAstroPartresults()\n");

	zeitGleichung_T.valid = false;
	deklination_T.valid = false;
	halberTag_T.valid = false;
}

double dayOfYear(time_t now)
{
	AP_DBG_TRACE(stderr,"dayOfYear(%ld)\n", now);

	struct tm *TM = localtime(&now);
	double result = (double)TM->tm_yday;
	_tnsutil_daylightSaving = TM->tm_isdst;
//	EPRINTF("dayOfYear(%d,%d)",result,_daylightSaving);
	if (result != lastDay) {
		initializeAstroPartresults();
		lastDay = result;
	}
	return result;
}

double Zeitgleichung(double T)
{
	AP_DBG_TRACE(stderr,"Zeitgleichung(%f)\t", T);

	if (zeitGleichung_T.valid) {
		AP_DBG_TRACE(stderr," cached :=%f\n", zeitGleichung_T.value);
		return zeitGleichung_T.value;
	}

	double T03 = 0.0337 * T;
	double T034 = T03 + 0.465;
	AP_DBG_TRACE(stderr, "T03: %f\tT034:%f\t", T03, T034);

	double T01 = 0.01787 * T;
	double T011 = T01 - 0.168;
	fprintf(stderr, "T01: %f\tT011:%f\t", T01, T011);

	AP_DBG_TRACE(stderr, "sin(T034): %f\tsin(T011):%f\t", SIN_(T034), SIN_(T011));

	double sin17 = 0.171 * SIN_(T034);
	double sin13 = 0.1299 * SIN_(T011);


	zeitGleichung_T.value = 0.0;
	zeitGleichung_T.value -= sin17;
	zeitGleichung_T.value -= sin13;

	zeitGleichung_T.valid = true;

	AP_DBG_TRACE(stderr,"calculated :=%f\n", zeitGleichung_T.value);
	return zeitGleichung_T.value;
}

double Deklination(double T)
{
	AP_DBG_TRACE(stderr,"Deklination(%f)\t", T);

	if (deklination_T.valid) {
		AP_DBG_TRACE(stderr," cached :=%f\n", deklination_T.value);
		return deklination_T.value;
	}

	double T80 = T - 80.086;
	double T0180 = 0.016906 * T80;
	double sinT01 = SIN_(T0180);

	AP_DBG_TRACE(stderr,"T80(%f)\tT0180(%f)\tsinT01(%f)\t", T80, T0180, sinT01);

	deklination_T.value  = 0.4095 * sinT01;
	deklination_T.valid = true;

	AP_DBG_TRACE(stderr," calculated :=%f\n", deklination_T.value);
	return deklination_T.value;
}


double halberTag(double T, double hight, double Breite)
{
	AP_DBG_TRACE(stderr,"halberTag_0(%f, %f, %f)\n", T, hight, Breite);

	AP_DBG_TRACE(stderr,"halberTag_0a1(%d)\n", halberTag_T.valid);
	AP_DBG_TRACE(stderr,"halberTag_0a2(%f)\n", halberTag_T.value);

	AP_DBG_TRACE(stderr,"halberTag_T(%f)\n", T);
	AP_DBG_TRACE(stderr,"halberTag_hight(%f)\n", hight);
	AP_DBG_TRACE(stderr,"halberTag_Breite(%f)\n", Breite);

	if (halberTag_T.valid) {
		AP_DBG_TRACE(stderr,"halberTag_0b(%f)\n", halberTag_T.value);
		return halberTag_T.value;
	}

	AP_DBG_TRACE(stderr,"halberTag_1(%f, %f, %f)\n", T, hight, Breite);

	double deklination = Deklination(T);

	double sin_deklination = SIN_(deklination);
	double sin_Breite = SIN_(Breite);
	double sin_BreiteXsinDeklination = sin_Breite * sin_deklination;

	double sin_hight = SIN_(hight);

	double cos_deklination = COS_(deklination);
	double cos_Breite = COS_(Breite);
	double cos_BreiteXcosDeklination = cos_Breite * cos_deklination;

	double sin_diff = sin_hight - sin_BreiteXsinDeklination;

	double sin_diff_cos_BreiteXcosDeklination = sin_diff / cos_BreiteXcosDeklination;

	AP_DBG_TRACE(stderr, "acos(%f)\n", sin_diff_cos_BreiteXcosDeklination);

//	double acos_result = 1.0;
	double acos_result = ACOS_(sin_diff_cos_BreiteXcosDeklination);
	fprintf(stderr, "acos_result = %f\n", acos_result);

	halberTag_T.value = 12.0 * acos_result / M_PI;
	halberTag_T.valid = true;

	return halberTag_T.value;
}	


double sunRiseLocal(double T, double height)
{
	AP_DBG_TRACE(stderr,"sunRiseLocal(%f, %f)\n", T, height);
	double result = 12.0 - halberTag(T, deg2rad(height), deg2rad(geographische_Breite)) - Zeitgleichung(T);
//	EPRINTF("sunRiseLocal(%f)", result);
	return result;
}

double sunSetLocal(double T, double height)
{
	double result = 12.0 + halberTag(T, deg2rad(height), deg2rad(geographische_Breite)) - Zeitgleichung(T);
//	EPRINTF("sunSetLocal(%f)",result);
	return result;
}

double localTimeOffset(void)
{
	double Zeitzone = 1.0 + (double)_tnsutil_daylightSaving;
	double result = Zeitzone - (geographische_Laenge / 15.0);
	return result;
}

double sunRise(double height, time_t now)
{
	AP_DBG_TRACE(stderr,"sunRise(%f, %ld)\n", height, now);
	double T = dayOfYear(now);
	return sunRiseLocal(T, height) + localTimeOffset();	
}

double sunSet(double height, time_t now)
{
	double T = dayOfYear(now);
	return sunSetLocal(T, height) + localTimeOffset();	
}



/*    struct tm {
    	  int     tm_sec;         // seconds 
    	  int     tm_min;         // minutes 
    	  int     tm_hour;        // hours 
    	  int     tm_mday;        // day of the month 
    	  int     tm_mon;         // month 
    	  int     tm_year;        // year 
    	  int     tm_wday;        // day of the week 
    	  int     tm_yday;        // day in the year 
    	  int     tm_isdst;       // daylight saving time 
      };		*/



t_timestruct *double2ts(double t, t_timestruct *buffer)
{
	buffer->hour = (int)t;
	buffer->minute = (int)((t -(double)buffer->hour)*60.0);
	buffer->second = (int)((t -(double)buffer->hour - (double)(buffer->minute/60.0))*3600.0);

	return buffer;
}

t_timestruct *getSunRise(void) {
	return &_tnsutil_SR;
}

t_timestruct *getSunSet(void) {
	return &_tnsutil_SS;
}

bool atDayTime(double height, time_t now)
{
	AP_DBG_TRACE(stderr,"astropak-0\n");
	long int thisDay = now / 86400;
	AP_DBG_TRACE(stderr,"astropak-1\n");
	struct tm *TM = localtime(&now);
	int result = TM->tm_yday;
	_tnsutil_daylightSaving = TM->tm_isdst;
	
	t_timestruct *sr, *ss;
	
	AP_DBG_TRACE(stderr,"astropak-2\n");
	sr = &_tnsutil_SR;
	ss = &_tnsutil_SS;
	
	AP_DBG_TRACE(stderr,"astropak-3\n");
	if (thisDay != _tnsutil_dayCount) {
		AP_DBG_TRACE(stderr,"astropak-4\n");
		double2ts(sunRise(height, now),&_tnsutil_SR);
		AP_DBG_TRACE(stderr,"astropak-5\n");
		double2ts(sunSet(height, now),&_tnsutil_SS);
		_tnsutil_dayCount = thisDay;
	}

	AP_DBG_TRACE(stderr,"astropak-6\n");
	EPRINTF8("astropak.cpp: sr: %02d:%02d:%02d now: %02d:%02d:%02d ss: %02d:%02d:%02d", 
			sr->hour, sr->minute, sr->second,
			TM->tm_hour, TM->tm_min, TM->tm_sec,
			ss->hour, ss->minute, ss->second);
	
	if ((TM->tm_hour > sr->hour) && (TM->tm_hour < ss->hour)) {
		return true;
	}
	if ((TM->tm_hour < sr->hour) || (TM->tm_hour > ss->hour)) {
		return false;
	}
	if (TM->tm_hour == sr->hour) {
		if (TM->tm_min > sr->minute) {
			return true;
		}
		if (TM->tm_min < sr->minute) {
			return false;
		}
		// test für sekunden
		if (TM->tm_sec > sr->second) {
			return true;
		}
		if (TM->tm_sec <= sr->second) {
			return false;
		}
	}
	if (TM->tm_hour == ss->hour) {
		if (TM->tm_min > ss->minute) {
			return false;
		}
		if (TM->tm_min < ss->minute) {
			return true;
		}
		// test für sekunden
		if (TM->tm_sec > sr->second) {
			return false;
		}
		if (TM->tm_sec <= sr->second) {
			return true;
		}
	}
	return false;
}


double modulo(double x, double y)
{
//	fprintf(stdout,"x:%f, y:%f\n",x,y);
	double f = frac(x/y);
	double r = f * y;

//	fprintf(stdout,"f:%f, r:%f\n",f,r);
	return r;
}


double Mod2Pi(double x, const char *funcname)	
{
//	fprintf(stdout,"caller: %s\n",funcname);
	return modulo(x, 2.0*M_PI);
}	


// Calculate Julian date: valid only from 1.3.1901 to 28.2.2100
double CalcJD(int day, int month, int year)
{
	double jd = 2415020.5 - 64.0; // 1.1.1900 - correction of algorithm
	if (month <= 2) { 
		year--; 
		month += 12; 
	}
	jd += floor( (double)(year-1900)*365.25 );
	jd += floor( (double)30.6001*(1.0+(double)month) );
//	fprintf(stdout,"day:%d, month:%d, year:%d\n",day,month,year);
	return(jd + day);
}

// Julian Date to Greenwich Mean Sidereal Time
double  GMST(double JD)
{
  double UT = frac(JD-0.5)*24; // UT in hours
//	fprintf(stdout,"UT:%f, frac(JD-0.5):%f\n",UT,frac(JD-0.5));
  double nJD = floor(JD-0.5)+0.5;   // JD at 0 hours UT
  double T = (nJD - 2451545.0)/36525.0;
  double T0 = 6.697374558 + T*(2400.051336 + (T*0.000025862));
  
  double tt = T0+(UT*1.002737909);
//  fprintf(stdout,"tt: %f\n",tt);
  return modulo(tt, 24.0);
}

// Convert Greenweek mean sidereal time to UT
double  GMST2UT(double JD, double gmst)
{
  JD = floor(JD-0.5)+0.5;   // JD at 0 hours UT
  double  T = (JD-2451545.0)/36525.0;
  double tt = 6.697374558 + T*(2400.051336 + T*0.000025862);
  double T0 = modulo(tt, 24);
  /* var UT = 0.9972695663*Mod((gmst-T0), 24.); */
  double UT = 0.9972695663*((gmst-T0));
  return (UT);
}

// Local Mean Sidereal Time, geographical longitude in radians, East is positive
double GMST2LMST(double gmst, double lon)
{
	double tt = gmst+rad2deg(lon)/15.0;
	double lmst = modulo(tt, 24.0);
	return( lmst );
}



// Rough refraction formula using standard atmosphere: 1015 mbar and 10°C
// Input true altitude in radians, Output: increase in altitude in degrees
double Refraction(double altitude, double pressure = 1015.0, double temperature = 10.0)
{
	double altdeg = rad2deg(altitude);
	if (altdeg < -2.0 || altdeg >= 90.0) {
		return(0);
	}
	if (altdeg > 15.0) {
		return( 0.00452*pressure/( (273.0+temperature) * tan(altitude)) );
	}	

	double y = altitude;
	double D = 0.0;
	double P = (pressure-80.0)/930.0;
	double Q = 0.0048*(temperature-10.0);
	double y0 = y;
	double D0 = D;

	for (int i=0; i<3; i++) {
		 double N = y+(7.31/(y+4.4));
		 N = 1.0/tan(deg2rad(N));
		 D = N*P/(60.0+Q*(N+39.0));
		 N = y-y0;
		 y0 = D-D0-N;
		 if ((N != 0.0) && (y0 != 0.0)) { 
			 N = y-N*(altitude+D-y)/y0; 
		 } else { 
			 N = altitude+D; 
		 }
		 y0 = y;
		 D0 = D;
		 y = N;
	}
	return( D ); // Hebung durch Refraktion in radians
}




// Transform ecliptical coordinates (lon/lat) to equatorial coordinates (RA/dec)
void t_SunPosition::Ecl2Equ(double TDT)
{
	double T = (TDT-2451545.0)/36525.0; // Epoch 2000 January 1.5
	
	double q = 23.0+(26+21.45/60.0)/60.0 + T*(-46.815 +T*(-0.0006 + T*0.00181) )/3600.0;
	double eps = deg2rad( q );
	
	double coseps = COS_(eps);
	double sineps = SIN_(eps);

	double sinlon = SIN_(lon);
	double y = (sinlon*coseps) - (tan(lat)*sineps);
	double x = COS_(lon);
	double tt = atan2( y, x);

	ra  = Mod2Pi(tt , "Ecl2Equ");

//	fprintf(stdout,"lat: %f, lon: %f, sinlon, %f\n",lat,lon);
//	dec = asin( (sin(lat)*coseps) + (cos(lat)*sineps*sinlon) );
	dec = ASIN_( sineps * sinlon );

//	fprintf(stdout,"ra: %f, tt:%f, rectascension: %f, dec:%f, declination:%f\n", ra, tt, rad2deg(ra)/15.0, dec, rad2deg(dec));
};

// Transform equatorial coordinates (RA/Dec) to horizonal coordinates (azimuth/altitude)
// Refraction is ignored
void t_SunPosition::Equ2Altaz(double TDT, double geolat, double lmst)
{
	double cosdec = COS_(dec);
	double sindec = SIN_(dec);
	double lha = lmst - ra;
	double coslha = COS_(lha);
	double sinlha = SIN_(lha);
	double coslat = COS_(geolat);
	double sinlat = SIN_(geolat);

	double N = -cosdec * sinlha;
	double D = (sindec * coslat) - (cosdec * coslha * sinlat);
	double tt = atan2(N, D);

	az = Mod2Pi( tt , "Equ2Altaz" );
	alt = ASIN_( (sindec * sinlat) + (cosdec * coslha * coslat) );
	
//	fprintf(stdout,"azimut: %f, declination:%f, altitude: %f\n", rad2deg(az), rad2deg(dec), rad2deg(alt));
};


// Calculate coordinates for Sun
// Coordinates are accurate to about 10s (right ascension) 
// and a few minutes of arc (declination)
void t_SunPosition::Calculate(double TDT, double geolat, double lmst)
{
	double D = TDT-2447891.5;

	double eg = deg2rad(279.403303);
	double wg = deg2rad(282.768422);
	double e  = 0.016713;
	double a  = 149598500; // km
	double diameter0 = deg2rad(0.533128); // angular diameter of Moon at a distance

//	double MSun = deg2rad(360.0)/365.242191*D+eg-wg;
	double MSun = (2.0*M_PI*D/365.242191) +eg -wg;
	double nu = MSun + deg2rad(360.0)/M_PI * e * SIN_(MSun);

	double nuwg = nu + wg;
	lon =  Mod2Pi(nuwg, "Calculate");
	lat = 0.0;
	anomalyMean = MSun;

	distance = (1.0-SQR(e))/(1.0+e*COS_(nu)); // distance in astronomical units
	diameter = diameter0/distance; // angular diameter in radians
	distance *= a;  					   // distance in km
	parallax = 6378.137/distance;  // horizonal parallax

//	fprintf(stdout,"sun longitude: %f, nu:%f, wg:%f, diameter:%f\n", rad2deg(lon), rad2deg(nu), rad2deg(wg), rad2deg(diameter)*60.0);

	Ecl2Equ(TDT);

	// Calculate horizonal coordinates of sun, if geographic positions is given
	if (geolat != 0.0 && lmst != 0.0) {
		Equ2Altaz(TDT, geolat, lmst);
	}
};




double t_SunPosition::Altitude(double lat, double lon, struct tm *ltime, int TZ)
{
//		ltime = localtime(&now);
	// int TZ = +1;	// GMT +1
	if (ltime->tm_year < 1900) { 
		ltime->tm_year += 1900;
		ltime->tm_mon += 1;
	}
	double JD0 = CalcJD( ltime->tm_mday, ltime->tm_mon, ltime->tm_year);
	double JD  = JD0 +( (double)ltime->tm_hour - (double)TZ + ((double)ltime->tm_min)/60.0 + ((double)ltime->tm_sec)/3600.0 ) /24.0;

//	fprintf(stdout,"julianisches datum: %f\n",JD);
	
	double deltaT = 65.0;		// difference among 'earth center' versus 'observered' time (TDT-UT), in seconds
	double TDT = JD + (deltaT / 24.0 / 3600.0);

	double gmst = GMST(JD);
	double lmst = GMST2LMST(gmst, lon);
	

	Calculate(TDT, lat, deg2rad(lmst*15));   // Calculate data for the Sun at given time

	double rf = Refraction(alt);
//	fprintf(stdout,"gmst: %f, lmst: %f, tdt: %f, rf: %f\n",gmst, lmst, TDT, rf);
	return rad2deg(alt) + rf;  // including refraction
};





/*



JD0 = CalcJD( eval(form.Day.value), eval(form.Month.value), eval(form.Year.value) );

 

sunCoor  = 
*/


/*__________________________________

	End-Of-File
  __________________________________*/

