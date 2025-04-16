#include "gnssdevice.h"

GnssDevice::GnssDevice(QObject *parent)
    : QObject{parent}
{

    serialport = new QSerialPort();
    serialport->setPortName(USBGNSSDEVICE);
    deviceopen = false;

}


GnssDevice::~GnssDevice()
{
    serialport->close();
    deviceopen = false;
}


bool GnssDevice::open()
{

    if (!serialport->open(QIODevice::ReadOnly))
    {
        return(false);
    }
    else
    {
        connect(serialport,SIGNAL(readyRead()),this,SLOT(slotReadData()));
        deviceopen = true;
        return(true);
    }

}


bool GnssDevice::isOpen()
{

    return(deviceopen);

}


void GnssDevice::slotReadData()
{

    while (serialport->canReadLine())
    {
        QByteArray message = serialport->readLine(500);
        QString stemp(message);
        emit newLine(stemp);
    }

}

