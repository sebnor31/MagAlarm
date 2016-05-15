#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "packetreader.h"
#include "calibration.h"
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
    Calibration *calib = 0;

    struct SensorGraph {
        QCPGraph *bx;
        QCPGraph *by;
        QCPGraph *bz;
        QCPAxisRect *axis;
    };

    SensorGraph refGraph;
    SensorGraph sensorGraph1;
    SensorGraph sensorGraph2;

/**************
 * Methods
 * ***********/

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setSensorPlot();
    void closeEvent(QCloseEvent *event);

private slots:
    void connStatusChanged(bool connected);
    void updateBatteryLevel(double batteryLevelVal);
    void updateSensorPlot(DataPacket packet);
    void on_calibButton_clicked();
    void calibFinished();

signals:
    void updateSamplingFreq(int msec);
};

#endif // MAINWINDOW_H
