#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "packetreader.h"
#include <QThread>
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

/**********************
 * Instance Variables
 * *******************/

private:
    Ui::MainWindow *ui;
    QThread pktThread;
    PacketManager *packetMgr = 0;

    struct SensorGraph {
        QCPGraph *bx;
        QCPGraph *by;
        QCPGraph *bz;
        QCPAxisRect *axis;
    };

    SensorGraph refGraph;
    SensorGraph sensor1Graph;
    SensorGraph sensor2Graph;

/**************
 * Methods
 * ***********/

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setSensorPlot();

private slots:
    void updateBatteryLevel(double batteryLevelVal);


};

#endif // MAINWINDOW_H
