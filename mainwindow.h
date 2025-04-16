#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QSettings>

#include "gnssdevice.h"
#include "utility.h"
#include "aircraft.h"
#include "genericserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);
    void readSettings();
    void writeSettings();
    int mwwidth;
    int mwheight;
    void createActions();
    void createMenus();
    QAction *exitAct;
    QAction *aboutAct;
    QMenu *fileMenu;
    QMenu *helpMenu;
    GnssDevice *myGnssDevice;
    bool gnssdevice;
    QMessageBox gnssMsgBox;
    bool validgnss;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;
    int latd;
    double latm;
    double lat;
    int lond;
    double lonm;
    double lon;
    double hmsl;
    double geoid;
    Aircraft *myplane;
    double dtemp;
    QLabel *labt00;
    QLabel *labt01;
    QLabel *labdate;
    QLabel *labtime;
    QLabel *labt10;
    QLabel *labt11;
    QLabel *labt12;
    QLabel *lablat;
    QLabel *lablon;
    QLabel *labht;
    QLabel *labt20;
    QLabel *labt21;
    QLabel *labt22;
    QLabel *labcog;
    QLabel *labsog;
    QLabel *labroc;
    void updateDateTime();
    void sendMessage();
    QChar leadingZero = QLatin1Char('0');
    GenericServer *server4080;

private slots:
    void slotNewGnssMsg(QString);
    void slotAbout();

};
#endif // MAINWINDOW_H
