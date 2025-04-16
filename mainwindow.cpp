#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Build the airplane
    myplane = new Aircraft(this);

    // Retrieve the saved user settings
    readSettings();

    // Build the GNSS device interface
    myGnssDevice = new GnssDevice(this);
    if (!myGnssDevice->open())
    {
        gnssdevice = false;
        QString stemp = "Cannot open USB2ILS device: ";
        stemp.append(USBGNSSDEVICE);
        stemp.append(".\n");
        stemp.append("SOXNav will run but cannot generate ILS signals.");
        gnssMsgBox.setText(stemp);
        gnssMsgBox.exec();
    }
    else
    {
        gnssdevice = true;
        connect(myGnssDevice,SIGNAL(newLine(QString)),this,SLOT(slotNewGnssMsg(QString)));
    }

    // Build the network server
    server4080 = new GenericServer(this,4080);

    // Window management
    setWindowTitle("SoxGNSSServer");
    setMinimumSize(150,100);
    resize(mwwidth,mwheight);
    QWidget *window = new QWidget(this);

    // Build the menu bar
    createActions();
    createMenus();

    // Build the GUI elements
    labt00 = new QLabel("UTC date");
    labdate = new QLabel();
    labdate->setFont(QFont("Helvetica",10,QFont::Bold));
    labdate->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    labt01 = new QLabel("UTC time");
    labtime = new QLabel();
    labtime->setFont(QFont("Helvetica",10,QFont::Bold));
    labtime->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    labt10 = new QLabel("Latitude (deg)");
    labt11 = new QLabel("Longitude (deg)");
    labt12 = new QLabel("Hgps (ft)");
    lablat = new QLabel();
    lablat->setFont(QFont("Helvetica",10,QFont::Bold));
    lablat->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    lablon = new QLabel();
    lablon->setFont(QFont("Helvetica",10,QFont::Bold));
    lablon->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    labht  = new QLabel();
    labht->setFont(QFont("Helvetica",10,QFont::Bold));
    labht->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    labt20 = new QLabel("COG (deg)");
    labt21 = new QLabel("SOG (knots)");
    labt22 = new QLabel("ROC (fpm)");
    labcog = new QLabel();
    labcog->setFont(QFont("Helvetica",10,QFont::Bold));
    labcog->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    labsog = new QLabel();
    labsog->setFont(QFont("Helvetica",10,QFont::Bold));
    labsog->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    labroc = new QLabel();
    labroc->setFont(QFont("Helvetica",10,QFont::Bold));
    labroc->setFrameStyle(QFrame::Panel|QFrame::Sunken);

    // Arrange the GUI elements
    //QVBoxLayout *layout = new QVBoxLayout;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(labt00,0,0);
    layout->addWidget(labt01,0,1);
    layout->addWidget(labdate,1,0);
    layout->addWidget(labtime,1,1);
    layout->addWidget(labt10,2,0);
    layout->addWidget(labt11,2,1);
    layout->addWidget(labt12,2,2);
    layout->addWidget(lablat,3,0);
    layout->addWidget(lablon,3,1);
    layout->addWidget(labht,3,2);
    layout->addWidget(labt20,4,0);
    layout->addWidget(labt21,4,1);
    layout->addWidget(labt22,4,2);
    layout->addWidget(labcog,5,0);
    layout->addWidget(labsog,5,1);
    layout->addWidget(labroc,5,2);

    // Finalize the overall GUI
    window->setLayout(layout);
    setCentralWidget(window);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{

    // Save the current user settings
    writeSettings();

}


void MainWindow::createActions()
{

    // File menu
    exitAct = new QAction("Exit",this);
    connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

    // Help menu
    aboutAct = new QAction("About",this);
    connect(aboutAct,SIGNAL(triggered()),this,SLOT(slotAbout()));

}


void MainWindow::createMenus()
{

    // File menu
    fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(exitAct);

    // Help menu
    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);

}


void MainWindow::slotAbout()
{

    QMessageBox msgBox;
    msgBox.setText("SoxGNSSServer 1.0\n"
                   "Qt5-based GNSS network server\n"
                   "from USB GNSS data\n"
                   "Author: John G. Sonntag\n"
                   "Released: 30 April 2025");
    msgBox.exec();

}


void MainWindow::readSettings()
{

    QSettings settings("nasa","soxgnssserver");
    mwwidth = settings.value("mwwidth",480).toInt();
    mwheight = settings.value("mwheight",750).toInt();

}


void MainWindow::writeSettings()
{

    QSettings settings("nasa","soxgnssserver");
    settings.setValue("mwwidth",width());
    settings.setValue("mwheight",height());

}


void MainWindow::slotNewGnssMsg(QString newgnss)
{
    QString stemp;

    // Parse the incoming message
    int size = newgnss.size();
    newgnss.resize(size-1);    // for some reason USB adds a carriage return to each line
    validgnss = false;
    stemp = newgnss.section(',',0,0);
    if (stemp=="$GPZDA")
    {
        if (nmeaChecksum(newgnss))
        {
            stemp = newgnss.section(',',2,2);
            day = stemp.toInt();
            stemp = newgnss.section(',',3,3);
            month = stemp.toInt();
            stemp = newgnss.section(',',4,4);
            year = stemp.toInt();
            myplane->setYmd(year,month,day);
            myplane->setDateAvailable(true);
            validgnss = true;
        }
    }
    else if (stemp=="$GPGGA")
    {
        if (nmeaChecksum(newgnss))
        {
            stemp = newgnss.section(',',1,1);
            dtemp = stemp.toDouble();
            hour = int(dtemp/10000.0);
            dtemp -= hour*10000.0;
            min = int(dtemp/100.0);
            sec = dtemp - min*100.0;
            myplane->setHms(hour,min,sec);
            stemp = newgnss.section(',',2,2);
            dtemp = stemp.toDouble();
            latd = int(dtemp/100.0);
            latm = dtemp - latd*100.0;
            lat = double(latd)+latm/60.0;
            stemp = newgnss.section(',',3,3);
            if (stemp=="S") lat = -lat;
            stemp = newgnss.section(',',4,4);
            dtemp = stemp.toDouble();
            lond = int(dtemp/100.0);
            lonm = dtemp - lond*100.0;
            lon = double(lond)+lonm/60.0;
            stemp = newgnss.section(',',5,5);
            if (stemp=="W") lon = -lon;
            myplane->setLatrLonr(lat*DEG2RAD,lon*DEG2RAD);
            stemp = newgnss.section(',',9,9);
            hmsl = stemp.toDouble();
            stemp = newgnss.section(',',11,11);
            geoid = stemp.toDouble();
            myplane->setHgpsm(hmsl+geoid);
            validgnss = true;
            updateDateTime();
            sendMessage();
        }
    }

}


void MainWindow::sendMessage()
{
    double dtemp;
    QString stemp1,stemp2;

    if (myplane->getDateAvailable())
    {
        stemp1 = "11,";
        stemp2 = QString("%1%2%3,").arg(myplane->getYear(),4,10,leadingZero)
                .arg(myplane->getMonth(),2,10,leadingZero)
                .arg(myplane->getDay(),2,10,leadingZero);
        stemp1.append(stemp2);
        stemp2 = QString("%1%2%3,").arg(myplane->getHour(),2,10,leadingZero)
                .arg(myplane->getMin(),2,10,leadingZero)
                .arg(myplane->getSec(),4,'f',1,leadingZero);
        stemp1.append(stemp2);
        dtemp = myplane->getLatd();
        stemp2 = QString("%1,").arg(dtemp,11,'f',7);
        stemp1.append(stemp2);
        dtemp = myplane->getLond();
        while (dtemp<0.0) dtemp+=360.0;
        stemp2 = QString("%1,").arg(dtemp,11,'f',7);
        stemp1.append(stemp2);
        dtemp = myplane->getHgpsft();
        stemp2 = QString("%1,").arg(dtemp,8,'f',2);
        stemp1.append(stemp2);
        //dtemp = 180.0;
        //while (dtemp<0.0) dtemp+= 360.0;
        stemp2 = QString("%1,").arg(myplane->getTrackd(),6,'f',2);
        stemp1.append(stemp2);
        stemp2 = QString("%1,").arg(myplane->getGsknots(),5,'f',1);
        stemp1.append(stemp2);
        dtemp = (myplane->getRocmps())*M2FT*60.0;
        stemp2 = QString("%1").arg(dtemp,6,'f',0);
        stemp1.append(stemp2);
        stemp1.append('\n');
        server4080->sendText(stemp1);
    }

}


void MainWindow::updateDateTime()
{
    QString stemp1;
    QString stemp2;

    if (myplane->getDateAvailable())
    {
        stemp1 = QString("%1-%2-%3  ").arg(myplane->getYear(),4,10,leadingZero)
                .arg(myplane->getMonth(),2,10,leadingZero)
                .arg(myplane->getDay(),2,10,leadingZero);
        labdate->setText(stemp1);
    }
    stemp2 = QString("%1:%2:%3").arg(myplane->getHour(),2,10,leadingZero)
            .arg(myplane->getMin(),2,10,leadingZero)
            .arg(myplane->getSec(),4,'f',1,leadingZero);
    labtime->setText(stemp2);

    // Position and velocity components
    stemp1 = QString("%1").arg(myplane->getLatd(),12,'f',8);
    lablat->setText(stemp1);
    stemp1 = QString("%1").arg(myplane->getLond(),13,'f',8);
    lablon->setText(stemp1);
    stemp1 = QString("%1").arg(myplane->getHgpsft(),7,'f',2);
    labht->setText(stemp1);
    stemp1 = QString("%1").arg(myplane->getTrackd(),5,'f',1);
    labcog->setText(stemp1);
    stemp1 = QString("%1").arg(myplane->getGsknots(),5,'f',1);
    labsog->setText(stemp1);
    stemp1 = QString("%1").arg(myplane->getRocfpm(),5,'f',0);
    labroc->setText(stemp1);

}
