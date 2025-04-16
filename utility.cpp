#include "utility.h"

#include <stdio.h>
#include <iostream>
using namespace std;

#include <QDebug>
#include <QList>


int mjd2cal(int mjd,double mjs,int *year,int *month,int *day,int *hour,int *min,double *sec)
{

    // Set constants
    const int YR0 = 1972;
    const int MJD0 = 41317;

    // Define number of days in each month
    QList<int> dayspermonth;
    dayspermonth.append(31);
    dayspermonth.append(28);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);

    // Compute year
    long ndays = mjd - MJD0;
    int nyrs = int(double(ndays)/365.25);
    *year = YR0 + nyrs;
    double dtemp = double(*year)/4.0 - double(*year/4);
    qDebug() << "dtemp =" << dtemp;

    // Adjust for leap day
    if (dtemp == 0)
    {
        dayspermonth[1] = 29;
        ndays++;
    }
    ndays -= long(nyrs*365.25);
    int i = 1;
    while (ndays>dayspermonth[i])
    {
        ndays -= dayspermonth[i];
        i++;
    }
    *month = i;
    *day = ndays-1;

    // Compute hour min sec
    *hour = int(mjs/3600.0);
    *min = int((mjs - *hour*3600.0)/60.0);
    *sec = mjs - *hour*3600.0 - *min*60.0;

    return(0);

}


int cal2mjd(int year,int month,int day,int hour,int min,double sec,
            int *mjd,double *mjs)
{

    // Set constants
    const int YR0 = 1972;
    const int MJD0 = 41317;

    // Define number of days in each month
    QList<int> dayspermonth;
    dayspermonth.append(31);
    dayspermonth.append(28);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);
    dayspermonth.append(30);
    dayspermonth.append(31);

    // Count days due to past full years, including leap days
    int nyrs = year-YR0;
    int ndays = nyrs*365;
    int i;
    double temp;
    for (i=YR0;i<year;i++)
    {
        temp = double(i)/4.0 - double(i/4);
        if (temp==0.0) ndays++;
    }

    // Add days due to past full months plus leap days
    for (i=0;i<(month-1);i++)
    {
        ndays += dayspermonth[i];
    }
    temp = double(year)/4.0 - double(year/4);
    if (temp==0 && month>2)
    {
        ndays++;
    }

    // Add days due to day of month
    ndays += day-1;

    // Compute MJD
    *mjd = MJD0+ndays;

    // Compute MJS and return
    *mjs = hour*3600.0+min*60.0+sec;
    return(0);

}


int parseiso8601(QString stime,int *year,int *month,int *day,
                 int *hour,int *min,double *sec)
{
    bool ok;
    int length;
    QString stemp,stemp1,stemp2;

    // Ensure format is correct and timezone is UTC
    length = stime.length();
    stemp1 = stime.mid(10,1);
    stemp2 = stime.mid(length-1,1);

    // Parse if all good and return 0
    if (stemp1=='T'&&stemp2=='Z')
    {
        stemp = stime.mid(0,4);
        *year = stemp.toInt(&ok,10);
        stemp = stime.mid(5,2);
        *month = stemp.toInt(&ok,10);
        stemp = stime.mid(8,2);
        *day = stemp.toInt(&ok,10);
        stemp = stime.mid(11,2);
        *hour = stemp.toInt(&ok,10);
        stemp = stime.mid(14,2);
        *min = stemp.toInt(&ok,10);
        stemp = stime.mid(17,6);
        *sec = stemp.toDouble(&ok);
        return(0);
    }

    // Otherwise return 1
    else
    {
        return(1);
    }

}


bool nmeaChecksum(QString nmeamessage)
{

    // Calculate checksum from the NMEA message
    int length = nmeamessage.length();
    char *buffer = NULL;
    buffer = new char[length+1];
    strcpy(buffer,nmeamessage.toLocal8Bit());
    int calcchecksum = calc_NMEA_Checksum(buffer,length);

    // Parse checksum from NMEA message
    QString stemp = nmeamessage.mid(length-3,2); // expect final character to be a newline, not part of checksum
    bool ok;
    int parsechecksum = stemp.toInt(&ok,16);  //hex text into decimal

    // Compare the results
    if (calcchecksum==parsechecksum) return(true);
    else return(false);

}


int calc_NMEA_Checksum( char *buf, int cnt )
{
    char Character;
    int Checksum = 0;
    int i;              // loop counter

    //foreach(char Character in sentence)
    for (i=0;i<cnt;++i)
    {
        Character = buf[i];
        switch(Character)
        {
            case '$':
                // Ignore the dollar sign
                break;
            case '*':
                // Stop processing before the asterisk
                i = cnt;
                continue;
            default:
                // Is this the first value for the checksum?
                if (Checksum == 0)
                {
                    // Yes. Set the checksum to the value
                    Checksum = Character;
                }
                else
                {
                    // No. XOR the checksum with this character's value
                    Checksum = Checksum ^ Character;
                }
                break;
        }
    }

    return(Checksum);
}


void sec2hms(double sec,int *h,int *m,double *s)
{

  double newsec = sec;
  int dow = (int)(newsec/86400.0);
  newsec -= dow*86400;
  int hr = (int)(newsec/3600.0);
  newsec -= hr*3600;
  int min = (int)(newsec/60.0);
  newsec -= min*60.0;
  *h = hr;
  *m = min;
  *s = newsec;
}


double timediff(timeval t1, timeval t2)
{
  double secs,usecs;

  secs = (double)(t1.tv_sec - t2.tv_sec);
  usecs = (double)(t1.tv_usec - t2.tv_usec);
  return(secs+usecs/1.0e6);
}


double gcdist(double lat1,double lon1,double lat2,double lon2)
{
  double dist;

  lon1 = -lon1;
  lon2 = -lon2;
  double dtemp = sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon1-lon2);
  if (dtemp>1.0) dtemp = 1.0;
  if (dtemp<-1.0) dtemp = -1.0;
  dist = acos(dtemp);
  //qDebug() << dtemp << lat1 << lat2 << lon1 << lon2;
  return(dist);

}


double gccourse1(double lat1,double lon1,double lat2,
                 double lon2)
{
  double dist,tc1;

  /*  Change longitude sign convention  */
  if (lon1<0.0)
    lon1 += 2.0*PI;
  if (lon2<0.0)
    lon2 += 2.0*PI;
  lon1 = -lon1;
  lon2 = -lon2;

  /*  Check for pole  */
  if (cos(lat1) < 0.00001)
  {
    if (lat1 > 0)
      tc1 = PI;
    else
      tc1 = 0;
  }

  /*  Check for meridian  */
  //else if (lon1 == lon2)
  else if (fabs(lon1-lon2)<(0.00001*DEG2RAD))
  {
    if (lat1>lat2)
    {
      tc1 = PI;
    }
    else
    {
      tc1 = 0.0;
    }
  }

  /*  Point 1 not a pole  */
  else
  {
    dist = acos(sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon1-lon2));
    tc1 = acos((sin(lat2)-sin(lat1)*cos(dist))/(sin(dist)*cos(lat1)));
    if (sin(lon2-lon1)>=0)
      tc1 = 2*PI-tc1;
  }

  /*  Return the result  */
  return(tc1);

}


void gcproject(double lat1,double lon1,double tc1,double dist,
                 double *lat2,double *lon2)
{
  double templon;

  // Compute point 2
  lon1 = -lon1;
  dist /= RAD2KM;
  *lat2 = asin(sin(lat1)*cos(dist)+cos(lat1)*sin(dist)*cos(tc1));
  templon = atan2(sin(tc1)*sin(dist)*cos(lat1),cos(dist)-sin(lat1)*sin(*lat2));
  /* lonb = (lona-templon+PI)%(2.0*PI) - PI;  */
  *lon2 = -(lon1 - templon);


}


void parsehms(double hms,int *hour,int *min,double *sec)
{
  *hour = hms/10000;
  double dtemp = hms-*hour*10000;
  *min = dtemp/100.0;
  dtemp = dtemp-*min*100.0;
  *sec=dtemp;
}


void parseymd(int ymd,int *year,int *month,int *day)
{
  *year = ymd/10000;
  int itemp = ymd-(*year*10000);
  *month = itemp/100.0;
  itemp = itemp-(*month*100);
  *day=itemp;
}


// This function considers the spherical triangle formed by from waypoint P1, current
// position P, and a perpendicular from P to the great circle path from P1 to P2, which
// intersectsthat path at point Pn.
// pflag = -1 if current position is behind P1 (perhaps approaching the line)
// pflag =  0 if current position is abeam the great circle arc P1->P2
// pflag =  1 if current position is beyond P2 (perhaps after departing the line)
//
void navcalc(double lat,double lon,double h,double lat1,double lon1,double h1,
             double lat2,double lon2,double h2,double *xtdiff,double *hdiff,int *pflag,
             double *tot)
{
  double lambda12,lambda1p,dtemp1,dtemp2,dtemp3,alpha12,alpha1p,alpha,p1p2,c,a,b,ht;
  double latn,lonn,lambda1n,alphan2;

  // Calculate longitude diffs
  lambda12 = lon2-lon1;
  lambda1p = lon-lon1;

  // Calculate heading P1->P2
  dtemp1 = cos(lat2)*sin(lambda12);
  dtemp2 = cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lambda12);
  alpha12 = atan2(dtemp1,dtemp2);
  //printf("alpha12 = %lf deg\n",alpha12/DEG2RAD);

  // Calculate heading P1->P
  dtemp1 = cos(lat)*sin(lambda1p);
  dtemp2 = cos(lat1)*sin(lat)-sin(lat1)*cos(lat)*cos(lambda1p);
  alpha1p = atan2(dtemp1,dtemp2);
  //printf("alpha1p = %lf deg\n",alpha1p/DEG2RAD);

  // Calculate spherical triangle angle alpha
  alpha = alpha1p-alpha12;
  while (alpha<-PI) alpha += 2.0*PI;
  while (alpha>PI) alpha -= 2.0*PI;
  //printf("alpha = %lf deg\n",alpha/DEG2RAD);

  // Calculate spherical triangle dist P1 to P2
  dtemp1 = cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lambda12);
  dtemp2 = cos(lat2)*sin(lambda12);
  dtemp3 = sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lambda12);
  p1p2 = atan2(sqrt(dtemp1*dtemp1+dtemp2*dtemp2),dtemp3);
  //printf("p1p2 = %lf km\n",p1p2*RAD2KM);

  // Calculate spherical triangle angle c (angular dist between P1 and P)
  dtemp1 = cos(lat1)*sin(lat)-sin(lat1)*cos(lat)*cos(lambda1p);
  dtemp2 = cos(lat)*sin(lambda1p);
  dtemp3 = sin(lat1)*sin(lat)+cos(lat1)*cos(lat)*cos(lambda1p);
  c = atan2(sqrt(dtemp1*dtemp1+dtemp2*dtemp2),dtemp3);
  //printf("c = %lf km\n",c*RAD2KM);

  // Calculate cross-track error a
  a = asin(sin(alpha)*sin(c));
  *xtdiff = a*RAD2KM*1000.0;
  //printf("cross-track error = %lf m\n",a*RAD2KM*1000.0);

  // Calculate spherical triangle angle b (dist along P1-Pn
  // Pn is the point along the P1-P2 great circle which P is normal to
  // Pn is the point where the aircraft would be if it were perfectly on
  // the P1-P2 great circle
  b = acos(cos(c)/cos(a));
  //printf("b = %lf nm\n",b*RAD2KM/NM2KM);

  // Quadrant check and very simple altitude diff calculation
  if ((alpha/DEG2RAD)<-90.0||alpha/DEG2RAD>90.0)  // Current point is before P1
  {
    *pflag=-1;
    *hdiff = h-h1;
  }
  else if (b>p1p2) // Current point is after P2
  {
    *pflag=1;
    *hdiff = h-h2;
  }
  else // Current point is between P1 and P2
  {
    *pflag=0;
    ht = h1+b*(h2-h1)/p1p2; // Target height linearly interpolated along great circle P1->P2
    *hdiff = h-ht;
  }

  // Calculate Pn coordinates and corresponding course angle
  // (track-on-track or tot, called alphan2 to conform to nomenclature here)
  if (*pflag==-1)
      gcproject(lat1,lon1,alpha12,-b*RAD2KM,&latn,&lonn);
  else
      gcproject(lat1,lon1,alpha12,b*RAD2KM,&latn,&lonn);
  if (*pflag==1)
  {
      lambda1n = lonn-lon1;
      dtemp1 = cos(lat1)*sin(lambda1n);
      dtemp2 = -cos(latn)*sin(lat1)+sin(latn)*cos(lat1)*cos(lambda1n);
      alphan2 = atan2(dtemp1,dtemp2);
      while (alphan2<0.0) alphan2+=2.0*PI;
      //printf("%lf\n",alphan2/DEG2RAD);
      *tot = alphan2;
  }
  else
  {
      alphan2 = gccourse1(latn,lonn,lat2,lon2);
      //printf("%lf\n",alphan2/DEG2RAD);
      *tot = alphan2;
  }


}



int geod2ps(double lat,double lon,double latc,double lon0,double k0,
                       double ae,double flat,double *x,double *y)
{
  int south;
  double e,t,tc,mc,rho;
  double dtemp1,dtemp2,dtemp3;

  // Convert to west longitude and radians
  //if (lond>180.0) lond-=360.0;
  //if (lon0d>180.0) lon0d-=360.0;
  while (lon>PI) lon -= 2.0*PI;
  while (lon0>PI) lon0 -= 2.0*PI;
  //lat  = latd*PI/180.0;
  //lon  = lond*PI/180.0;
  //latc = latcd*PI/180.0;
  //lon0 = lon0d*PI/180.0;

  // Check for south polar aspect
  south = 0;
  if (latc < 0.0)
  {
    lat  *= -1.0;
    lon  *= -1.0;
    latc *= -1.0;
    lon0 *= -1.0;
    south = 1;
  }
  //printf("%lf %lf %lf %lf %d\n",lat,lon,latc,lon0,south);
  //printf("ae %lf  flat %lf\n",ae,flat);

  // Compute the ellipsoid's eccentricity
  e = sqrt(1.0 - (1.0-flat)*(1.0-flat));

  //  Compute t
  dtemp1 = (1.0-sin(lat))/(1.0+sin(lat));
  dtemp2 = pow(((1.0+e*sin(lat))/(1.0-e*sin(lat))),e);
  t = sqrt(dtemp1*dtemp2);

  // Compute rho
  if (latc==(PI/2.0)||latc==-(PI/2.0))
  {
    dtemp1 = pow((1.0+e),(1.0+e));
    dtemp2 = pow((1.0-e),(1.0-e));
    dtemp3 = sqrt(dtemp1*dtemp2);
    rho = 2.0*ae*k0*t/dtemp3;
  }
  else
  {
    dtemp1 = (1.0-sin(latc))/(1.0+sin(latc));
    dtemp2 = pow(((1.0+e*sin(latc))/(1.0-e*sin(latc))),e);
    tc = sqrt(dtemp1*dtemp2);
    dtemp1 = sqrt(1.0-e*e*sin(latc)*sin(latc));
    mc = cos(latc)/dtemp1;
    rho = ae*mc*t/tc;
  }

  // Compute x and y
  *x = rho*sin(lon-lon0);
  *y = -rho*cos(lon-lon0);

  // Check for south polar aspect
  if (south)
  {
    *x *= -1.0;
    *y *= -1.0;
  }

  // Return
  return(0);

}


int geod2utm(double lat,double lon,double loncm,double *y,double *x,int *zone)
{
  double S,nu,k0,FN,FE,ep2,ep4,ep6,ep8,n,n2,n3,n4,n5,e2,lon0;
  double a_prime,b_prime,c_prime,d_prime,e_prime;
  double dlon,dlon2,dlon3,dlon4,dlon5,dlon6,dlon7,dlon8;
  double sinlat,coslat,cos2lat,cos3lat,cos4lat,cos5lat,cos6lat,cos7lat,cos8lat;
  double tanlat,tan2lat,tan4lat,tan6lat,tan8lat;
  double T1,T2,T3,T4,T5,T6,T7,T8,T9;

  //cout << "LON0=" << LON0 << '\n';

  // Convert input lat/lon to radians
  //lat *= PI/180.0;
  //lon *= PI/180.0;

   /*  Check input lat and lon  */
  if ( lon < 0.0 )
    lon += 2.0*PI;
  if ( lon < 0.0 || lon > 2.0*PI )
  {
    *zone = -99;
    *x = -999.99;
    *y = -999.99;
    return(-1);
  }
  if ( lat < -PI/2.0 || lat > PI/2.0 )
  {
    *zone = -99;
    *x = -999.99;
    *y = -999.99;
    return(-1);
  }

  // Compute and store one-time constants
  //if (firstcoords)
  //{

    /*  Define constants  */
    // For this application we don't use false northing/eastings
    // since they confuse things like the equator crossing
    k0 = 0.9996;
    //if ( lat >= 0.0 )
    //  FN = 0.0;
    //else
    //  FN = 10000000.0;
    //FE = 500000.0;
    FN = 0.0;
    FE = 0.0;

    /*  Compute primed quantities  */
    n = FLAT/(2.0-FLAT);
    e2 = FLAT*(2.0-FLAT);
    ep2 = e2/(1.0-FLAT)/(1.0-FLAT);
    ep4 = ep2*ep2;
    ep6 = ep4*ep2;
    ep8 = ep6*ep2;
    n2 = n*n;
    n3 = n2*n;
    n4 = n3*n;
    n5 = n4*n;
    a_prime = AE*(1.0 - n + 5.0/4.0*(n2-n3) + 81.0/64.0*(n4-n5));
    b_prime = 3.0/2.0*AE*(n - n2 + 7.0/8.0*(n3-n4) + 55.0/64.0*n5);
    c_prime = 15.0/16.0*AE*(n2 - n3 + 3.0/4.0*(n4-n5));
    d_prime = 35.0/48.0*AE*(n3 - n4 + 11.0/16.0*n5);
    e_prime = 315.0/512.0*AE*(n4 - n5);

    /*  Compute zone number and central meridian  */
    //lon_deg = lon*180.0/PI;
    //lon0_deg = 6.0*(int)(1+lon_deg/6.0) - 3.0;
    //lon0 = lon0_deg*PI/180.0;

    //firstcoords = false;
  //}

  // Set the central meridian
  //lon0 = LON0*PI/180.0;
  lon0 = loncm;

  /*  Compute meriodonal arc  */
  nu = AE/sqrt(1.0 - e2*sin(lat)*sin(lat));
  S = a_prime*lat - b_prime*sin(2.0*lat) + c_prime*sin(4.0*lat)
      - d_prime*sin(6.0*lat) + e_prime*sin(8.0*lat);

  // Compute the delta-lons
   //*zone = (int)(1+lon_deg/6.0) + 30;
   //if (*zone > 60 )
   //  *zone -= 60;
   dlon = lon - lon0;
   dlon2 = dlon*dlon;
   dlon3 = dlon2*dlon;
   dlon4 = dlon2*dlon2;
   dlon5 = dlon4*dlon;
   dlon6 = dlon4*dlon2;
   dlon7 = dlon6*dlon;
   dlon8 = dlon6*dlon2;

   /*  Compute the Ts  */
   sinlat = sin(lat);
   coslat = cos(lat);
   cos2lat = coslat*coslat;
   cos3lat = cos2lat*coslat;
   cos4lat = cos2lat*cos2lat;
   cos5lat = cos4lat*coslat;
   cos6lat = cos4lat*cos2lat;
   cos7lat = cos6lat*coslat;
   cos8lat = cos6lat*cos2lat;
   tanlat = tan(lat);
   tan2lat = tanlat*tanlat;
   tan4lat = tan2lat*tan2lat;
   tan6lat = tan4lat*tan2lat;
   tan8lat = tan6lat*tan2lat;
   T1 = S*k0;
   T2 = nu*sinlat*coslat*k0/2.0;
   T3 = nu*sinlat*cos3lat*k0/24*(5.0 - tan2lat + 9.0*ep2*cos2lat
        + 4.0*ep4*cos4lat);
   T4 = nu*sinlat*cos5lat*k0/720.0*(61.0 - 58.0*tan2lat + tan4lat
        + 270.0*ep2*cos2lat - 330.0*tan2lat*ep2*cos2lat + 445.0*ep4*cos4lat
        + 324*ep6*cos6lat - 680.0*tan2lat*ep4*cos4lat + 88.0*ep8*cos8lat
        - 600.0*tan2lat*ep6*cos6lat - 192.0*tan2lat*ep8*cos8lat);
   T5 = nu*sinlat*cos7lat*k0/40320.0*(1385.0 - 3111.0*tan2lat - 543.0*tan4lat
        - tan6lat);
   T6 = nu*coslat*k0;
   T7 = nu*cos3lat*k0/6.0*(1.0 - tan2lat + ep2*cos2lat);
   T8 = nu*cos5lat*k0/120.0*(5.0 - 18.0*tan2lat + tan4lat + 14.0*ep2*cos2lat
        - 58.0*tan2lat*ep2*cos2lat + 13.0*ep4*cos4lat + 4.0*ep6*cos6lat
        - 64.0*tan2lat*ep4*cos4lat - 24.0*tan2lat*ep6*cos6lat);
   T9 = nu*cos7lat*k0/5040.0*(61.0 - 479.0*tan2lat + 179.0*tan4lat
        - tan6lat);

   /*  Compute northing and easting  */
   *y = FN + T1 + dlon2*T2 + dlon4*T3 + dlon6*T4 + dlon8*T5;
   *x  = FE + dlon*T6 + dlon3*T7 + dlon5*T8 + dlon7*T9;
   return(0);

   //*y = lat;
   //*x = lon;
   //*zone = 99;
   //return(0);
 }


void projmercator(double lat,double lon,double *x, double *y)
{
  *x = AE*lon;
  *y = AE*log(tan(PI/4.0+lat/2.0));
}


void inversemercator(double x,double y,double *lat,double *lon)
{
  double dtemp;

  dtemp = exp(-y/AE);
  *lat = PI/2.0-2.0*atan(dtemp);
  *lon = x/AE;

}


double trackddiff(double track1d,double track2d)
{

    // Ensure both input track angles are 0-360
    double ntrack1d = track1d;
    double ntrack2d = track2d;
    //while (ntrack1d<0.0) ntrack1d+=360.0;
    //while (ntrack1d>-360.0) ntrack1d-=360.0;
    //while (ntrack2d<0.0) ntrack2d+=360.0;
    //while (ntrack2d>-360.0) ntrack2d-=360.0;

    // Handle cases where the two headings are on opposite sides of north (0 deg)
    //if (ntrack1d>180.0 && ntrack2d<180.0) ntrack2d+=360.0;
    //if (ntrack1d<180.0 && ntrack2d>180.0) ntrack1d+=360.0;

    // Calculate track angle difference and ensure -180<result<180
    double tdiffd = ntrack1d-ntrack2d;
    while (tdiffd<-180.0) tdiffd+=360.0;
    while (tdiffd>180.0) tdiffd-=360.0;
    return(tdiffd);

}

