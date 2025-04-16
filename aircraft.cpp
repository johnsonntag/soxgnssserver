#include "aircraft.h"

Aircraft::Aircraft(QObject *parent)
{

    year = 0;
    month = 0;
    day = 0;
    hour = 0;
    min = 0;
    sec = 0.0;
    mjd = 0;
    mjdlast = 0;
    mjs = 0.0;
    mjslast = 0.0;
    latr = 0.0;
    lonr = 0.0;
    latrlast = 0.0;
    lonrlast = 0.0;
    hgpsm = 0.0;
    hgpsmlast = 0.0;
    trackr = 0.0;
    oldtrackr = 0.0;
    gsmps = 0.0;
    dateavailable = false;
    gpstimeavailable = false;
    qrocmps.clear();
    qgsmps.clear();
    qlatr.clear();
    qlonr.clear();
    sumrocmps = 0.0;
    sumgsmps = 0.0;
    nrocmean = 20; // we average ROC over 10 updates nominally 1 sec
    ngsmean = 10;  // we average groundspeed over 10 updates
    npos = 20;  // We compute track angle across this many position updates

}



void Aircraft::setDateAvailable(bool newdateavailable)
{
    dateavailable = newdateavailable;
}


void Aircraft::setGpsTimeAvailable(bool newgpstimeavailable)
{
    gpstimeavailable = newgpstimeavailable;
}


void Aircraft::setYmd(int newyear,int newmonth, int newday)
{
    year = newyear;
    month = newmonth;
    day = newday;
    mjdlast = mjd;
    double mjstemp;
    int flag1 = cal2mjd(year,month,day,0,0,0.0,&mjd,&mjstemp);

}


void Aircraft::setHms(int newhour,int newmin, double newsec)
{
    hour = newhour;
    min = newmin;
    sec = newsec;
    mjslast = mjs;
    //int flag1 = cal2mjd(year,month,day,hour,min,sec,&mjd,&mjs);
    mjs = hour*3600.0+min*60.0+sec;

    // Check for time-of-day rollover not (yet) accompanied by a date rollover
    // this occurs because $GPZDA comes in at a lower rate than $GPGGA
    int itemp;
    double dtemp;
    if (mjs<mjslast&&mjd==mjdlast)
    {
        mjd++;
        int flag = mjd2cal(mjd,0.0,&year,&month,&day,&itemp,&itemp,&dtemp);
        //qDebug() << "rollover in mjs!";
    }

}


void Aircraft::setLatrLonr(double newlatr,double newlonr)
{

    // Update lat and lon
    latrlast = latr;
    lonrlast = lonr;
    latr = newlatr;
    lonr = newlonr;
    while (lonr<0.0) lonr+=2.0*PI;
    qlatr.enqueue(latr);
    qlonr.enqueue(lonr);
    if (qlatr.size()>npos)
    {
        qlatr.dequeue();
        qlonr.dequeue();
    }

    // Calculate speed over ground
    dist1 = 1000.0*RAD2KM*gcdist(latrlast,lonrlast,latr,lonr);
    dts = (mjd-mjdlast)*604800.0+(mjs-mjslast);
    gsmps1 = dist1/dts;
    sumgsmps += gsmps1;
    qgsmps.enqueue(gsmps1);
    if (qgsmps.size()>ngsmean)
    {
        sumgsmps -= qgsmps.first();
        qgsmps.dequeue();
    }
    //qDebug() << sumgsmps << qgsmps.size();
    sumgsmps = fabs(sumgsmps);
    setGsmps(sumgsmps/double(qgsmps.size()));

    // Calculate course over ground
    track1r = gccourse1(qlatr.first(),qlonr.first(),latr,lonr);
    //qDebug() << qlatr.size() << qlonr.size();
    setTrackr(track1r);

}


void Aircraft::setHgpsm(double newhgpsm)
{
    hgpsmlast = hgpsm;
    hgpsm = newhgpsm;
    dts = (mjd-mjdlast)*604800.0+(mjs-mjslast);
    rocmps1 = (hgpsm-hgpsmlast)/dts;
    sumrocmps += rocmps1;
    qrocmps.enqueue(rocmps1);
    if (qrocmps.size()>nrocmean)
    {
        sumrocmps -= qrocmps.first();
        qrocmps.dequeue();
    }
    setRocmps(sumrocmps/double(qrocmps.size()));
    //qDebug() << hgpsm << hgpsmlast << dts;
    //qDebug() << mjd << mjdlast << mjs << mjslast;
    //qDebug() << sumrocmps << qrocmps.size();
}


void Aircraft::setHmslm(double newhmslm)
{
    hmslm = newhmslm;
}


void Aircraft::setTrackr(double newtrackr)
{
    oldtrackr = trackr;
    trackr = newtrackr;
    while ((trackr*RAD2DEG)<0.0) trackr += 2.0*PI;
}


void Aircraft::setGsmps(double newgsmps)
{
    gsmps = newgsmps;
}


void Aircraft::setRocmps(double newrocmps)
{
    rocmps = newrocmps;
}


bool Aircraft::getDateAvailable()
{
    return(dateavailable);
}


bool Aircraft::getGpsTimeAvailable()
{
    return(gpstimeavailable);
}


int Aircraft::getYear()
{
    return(year);
}


int Aircraft::getMonth()
{
    return(month);
}


int Aircraft::getDay()
{
    return(day);
}

int Aircraft::getHour()
{
    return(hour);
}


int Aircraft::getMin()
{
    return(min);
}


double Aircraft::getSec()
{
    return(sec);
}


double Aircraft::getSOD()
{

    return(3600.0*double(hour)+60.0*double(min)+sec);

}


int Aircraft::getMjd()
{
    return(mjd);
}


double Aircraft::getMjs()
{
    return(mjs);
}


double Aircraft::getLatr()
{
    return(latr);
}


double Aircraft::getLatd()
{
    return(latr/DEG2RAD);
}


double Aircraft::getLonr()
{
    return(lonr);
}


double Aircraft::getLond()
{
    return(lonr/DEG2RAD);
}


double Aircraft::getHgpsm()
{
    return(hgpsm);
}


double Aircraft::getHgpsft()
{
    return(hgpsm*M2FT);
}


double Aircraft::getTrackr()
{
    return(trackr);
}


double Aircraft::getTrackd()
{
    return(trackr/DEG2RAD);
}


double Aircraft::getTrackRaterps()
{
    double dtrackr = trackr-oldtrackr;
    double tdiff = (mjd-mjdlast)*86400.0+(mjs-mjslast);
    if (dtrackr<-PI) dtrackr = dtrackr+2.0*PI;
    if (dtrackr>PI) dtrackr = dtrackr-2.0*PI;
    //qDebug() << mjd << mjs << mjdlast << mjslast << tdiff;
    return((dtrackr)/tdiff);

}


double Aircraft::getGsmps()
{
    return(gsmps);
}


double Aircraft::getGsknots()
{
    return(gsmps*M2FT*3600.0/6080.0);
}


double Aircraft::getRocmps()
{
    return(rocmps);
}


double Aircraft::getRocfpm()
{
    return(rocmps*M2FT*60.0);
}


void Aircraft::setGpsMinusUtc(int newgpsminusutcsec)
{
    gpsminusutcsec = newgpsminusutcsec;
}


void Aircraft::setHmslAvailable(bool newhmslavailable)
{
    hmslavailable = newhmslavailable;
}
