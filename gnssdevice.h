#ifndef GNSSDEVICE_H
#define GNSSDEVICE_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

#include "constants.h"

class GnssDevice : public QObject
{
    Q_OBJECT
public:
    explicit GnssDevice(QObject *parent = nullptr);
    ~GnssDevice();
    bool open();
    bool isOpen();

signals:
    void newLine(QString);

private:
    QSerialPort *serialport;
    bool deviceopen;

private slots:
    void slotReadData();

};

#endif // GNSSDEVICE_H
