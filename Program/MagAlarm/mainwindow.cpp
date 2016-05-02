#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    packetMgr = new PacketManager();
    packetMgr->moveToThread(&pktThread);

    connect(&pktThread, SIGNAL(started()), packetMgr, SLOT(start()));
    connect(&pktThread, SIGNAL(finished()), packetMgr, SLOT(deleteLater()), Qt::DirectConnection);

    connect(packetMgr, SIGNAL(batteryLevel(double)), this, SLOT(updateBatteryLevel(double)));

    pktThread.start();
}



void MainWindow::setSensorPlot()
{
    ui->sensorPlot->plotLayout()->clear();

}

void MainWindow::updateBatteryLevel(double batteryLevelVal)
{
    ui->batteryLevel->setValue(batteryLevelVal);
}


MainWindow::~MainWindow()
{
    delete ui;

    pktThread.quit();
    pktThread.wait();
}
