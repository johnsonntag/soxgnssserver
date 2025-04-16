
#include <QDebug>
#include <QString>

#include <sys/time.h>
#include <math.h>

#include "constants.h"
//#include "datastructures.h"

int mjd2cal(int, double,int *,int *,int *,int *,int *,double *);
int cal2mjd(int,int,int,int,int,double,int *,double *);
bool nmeaChecksum(QString);
int parseiso8601(QString,int *,int *,int *,int *,int *,double *);
int calc_NMEA_Checksum(char *,int);
double timediff(timeval, timeval);
double gcdist(double lat1,double lon1,double lat2,double lon2);
double gccourse1(double lat1,double lon1,double lat2,double lon2);
void gcproject(double lat1,double lon1,double tc1,double dist,
                 double *lat2,double *lon2);
void navcalc(double lat,double lon,double h,double lat1,double lon1,double h1,
             double lat2,double lon2,double h2,double *xtdiff,double *hdiff,
             int *pflag,double *tot);
void sec2hms(double sec,int *h,int *m,double *s);
void parsehms(double hms,int *hour,int *min,double *sec);
void parseymd(int hms,int *year,int *month,int *day);
int geod2ps(double latd,double lond,double latcd,double lon0d,double k0,
                       double ae,double flat,double *x,double *y);

int geod2utm(double lat,double lon,double loncm,double *y,double *x,int *zone);
void projmercator(double, double, double *, double *);
void inversemercator(double, double, double *, double *);
double trackddiff(double,double);

