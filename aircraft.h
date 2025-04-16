#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <QObject>
#include <QQueue>

#include "constants.h"
#include "utility.h"

class Aircraft
{
public:
    explicit Aircraft(QObject *parent=0);
    void setDateAvailable(bool);
    void setGpsTimeAvailable(bool);
    void setGpsMinusUtc(int);
    void setHmslAvailable(bool);
    void setModeFlagAvailable(bool);
    void setYmd(int,int,int);
    void setHms(int,int,double);
    void setLatrLonr(double,double);
    void setHorzUncm(double);
    void setX(double);
    void setY(double);
    void setHgpsm(double);
    void setHmslm(double);
    void setVertUncm(double);
    void setTrackr(double);
    void setGsmps(double);
    void setRocmps(double);
    bool getDateAvailable();
    bool getGpsTimeAvailable();
    int getYear();
    int getMonth();
    int getDay();
    int getHour();
    int getMin();
    double getSec();
    int getMjd();
    double getMjs();
    double getSOD();
    double getLatr();
    double getLatd();
    double getLonr();
    double getLond();
    double getHorzUncm();
    double getHorzUncft();
    double getHgpsm();
    double getHgpsft();
    double getVertUncm();
    double getVertUncft();
    double getTrackr();
    double getTrackd();
    double getTrackRaterps();
    double getGsmps();
    double getGsknots();
    double getRocmps();
    double getRocfpm();

private:
    bool dateavailable;
    bool gpstimeavailable;
    int gpsminusutcsec;
    bool hmslavailable;
    int year;
    int month;
    int day;
    int hour;
    int min;
    int mjd;
    int mjdlast;
    double mjs;
    double mjslast;
    double sec;
    double latr;
    double lonr;
    double latrlast;
    double lonrlast;
    double horzuncm;
    double hgpsm;
    double hgpsmlast;
    double hmslm;
    double vertuncm;
    double trackr;
    double oldtrackr;
    double gsmps;
    double rocmps;
    QQueue<double> qrocmps;
    QQueue<double> qgsmps;
    QQueue<double> qlatr;
    QQueue<double> qlonr;
    double sumrocmps;
    double sumgsmps;
    double rocmps1;
    int nrocmean;
    int ngsmean;
    double dist1;
    double dts;
    double gsmps1;
    double track1r;
    int npos;

};

#endif // AIRCRAFT_H
