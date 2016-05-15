#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connStatusChanged(false); // Set initial device status to not connected

    packetMgr = new PacketManager();
    packetMgr->moveToThread(&pktThread);

    connect(&pktThread, SIGNAL(started()), packetMgr, SLOT(start()));
    connect(&pktThread, SIGNAL(finished()), packetMgr, SLOT(deleteLater()), Qt::DirectConnection);

    connect(packetMgr, SIGNAL(connectedSig(bool)), this, SLOT(connStatusChanged(bool)));
    connect(packetMgr, SIGNAL(batteryLevel(double)), this, SLOT(updateBatteryLevel(double)));
    connect(packetMgr, SIGNAL(magFieldData(DataPacket)), this, SLOT(updateSensorPlot(DataPacket)));
    connect(this, SIGNAL(updateSamplingFreq(int)), packetMgr, SLOT(samplingFreq(int)));

    pktThread.start();

    setSensorPlot();
}


void MainWindow::on_calibButton_clicked()
{
    emit updateSamplingFreq(10);    // Set sampling freq to 100 Hz (10 ms)

    // User selects the file where raw mag data will be saved
    QString rawMagFilePath = QFileDialog::getSaveFileName(this, "Save Calibration Data", "C:/", "Text Files (*.txt)");
    calib = new Calibration(rawMagFilePath, this);
    connect(packetMgr, SIGNAL(magFieldData(DataPacket)), calib, SLOT(saveRawMagField(DataPacket)));

    // Perform saving for a fix duration
    int calibDurationMs = 5000;
    QTimer::singleShot(calibDurationMs, this, SLOT(calibFinished()));
}

void MainWindow::calibFinished()
{
    // Perfrom closing and cleanup of calibration
    disconnect(packetMgr, SIGNAL(magFieldData(DataPacket)), calib, SLOT(saveRawMagField(DataPacket)));
    calib->stopSaving();
    delete calib;

    // Reset sampling freq to 1 Hz (1000 ms)
    emit updateSamplingFreq(1000);
}

/************************************
 * Manage Plotting of Magnetic Field
 **************************************/
void MainWindow::setSensorPlot()
{
    ui->sensorPlot->plotLayout()->clear();

    // Setup Localization plane axis
    QCPRange magRange(0, 65536);
    QCPRange initTimeRange(0, 60);

    refGraph.axis = new QCPAxisRect(ui->sensorPlot);
    refGraph.axis->setupFullAxesBox(true);
    refGraph.axis->axis(QCPAxis::atTop)->setLabel("Reference");
    refGraph.axis->axis(QCPAxis::atBottom)->setLabel("time (s)");
    refGraph.axis->axis(QCPAxis::atLeft)->setRange(magRange);
    refGraph.axis->axis(QCPAxis::atBottom)->setRange(initTimeRange);

    sensorGraph1.axis = new QCPAxisRect(ui->sensorPlot);
    sensorGraph1.axis->setupFullAxesBox(true);
    sensorGraph1.axis->axis(QCPAxis::atTop)->setLabel("Sensor 1");
    sensorGraph1.axis->axis(QCPAxis::atBottom)->setLabel("time (s)");
    sensorGraph1.axis->axis(QCPAxis::atLeft)->setRange(magRange);
    sensorGraph1.axis->axis(QCPAxis::atBottom)->setRange(initTimeRange);

    sensorGraph2.axis = new QCPAxisRect(ui->sensorPlot);
    sensorGraph2.axis->setupFullAxesBox(true);
    sensorGraph2.axis->axis(QCPAxis::atTop)->setLabel("Sensor 2");
    sensorGraph2.axis->axis(QCPAxis::atBottom)->setLabel("time (s)");
    sensorGraph2.axis->axis(QCPAxis::atLeft)->setRange(magRange);
    sensorGraph2.axis->axis(QCPAxis::atBottom)->setRange(initTimeRange);

    // Add vertically stacked plots
    ui->sensorPlot->plotLayout()->addElement(0, 0, refGraph.axis);
    ui->sensorPlot->plotLayout()->addElement(1, 0, sensorGraph1.axis);
    ui->sensorPlot->plotLayout()->addElement(2, 0, sensorGraph2.axis);

    // Create and add graphs
    refGraph.bx = ui->sensorPlot->addGraph(refGraph.axis->axis(QCPAxis::atBottom), refGraph.axis->axis(QCPAxis::atLeft));
    refGraph.by = ui->sensorPlot->addGraph(refGraph.axis->axis(QCPAxis::atBottom), refGraph.axis->axis(QCPAxis::atLeft));
    refGraph.bz = ui->sensorPlot->addGraph(refGraph.axis->axis(QCPAxis::atBottom), refGraph.axis->axis(QCPAxis::atLeft));

    sensorGraph1.bx = ui->sensorPlot->addGraph(sensorGraph1.axis->axis(QCPAxis::atBottom), sensorGraph1.axis->axis(QCPAxis::atLeft));
    sensorGraph1.by = ui->sensorPlot->addGraph(sensorGraph1.axis->axis(QCPAxis::atBottom), sensorGraph1.axis->axis(QCPAxis::atLeft));
    sensorGraph1.bz = ui->sensorPlot->addGraph(sensorGraph1.axis->axis(QCPAxis::atBottom), sensorGraph1.axis->axis(QCPAxis::atLeft));

    sensorGraph2.bx = ui->sensorPlot->addGraph(sensorGraph2.axis->axis(QCPAxis::atBottom), sensorGraph2.axis->axis(QCPAxis::atLeft));
    sensorGraph2.by = ui->sensorPlot->addGraph(sensorGraph2.axis->axis(QCPAxis::atBottom), sensorGraph2.axis->axis(QCPAxis::atLeft));
    sensorGraph2.bz = ui->sensorPlot->addGraph(sensorGraph2.axis->axis(QCPAxis::atBottom), sensorGraph2.axis->axis(QCPAxis::atLeft));

    // Set graph parameters
    refGraph.bx->setPen(QPen(Qt::red));
    refGraph.by->setPen(QPen(Qt::blue));
    refGraph.bz->setPen(QPen(Qt::green));

    sensorGraph1.bx->setPen(QPen(Qt::red));
    sensorGraph1.by->setPen(QPen(Qt::blue));
    sensorGraph1.bz->setPen(QPen(Qt::green));

    sensorGraph2.bx->setPen(QPen(Qt::red));
    sensorGraph2.by->setPen(QPen(Qt::blue));
    sensorGraph2.bz->setPen(QPen(Qt::green));
}

void MainWindow::updateSensorPlot(DataPacket packet)
{
    refGraph.bx->addData(packet.time, packet.refMag.at(0));
    refGraph.by->addData(packet.time, packet.refMag.at(1));
    refGraph.bz->addData(packet.time, packet.refMag.at(2));

    sensorGraph1.bx->addData(packet.time, packet.sensorMag1.at(0));
    sensorGraph1.by->addData(packet.time, packet.sensorMag1.at(1));
    sensorGraph1.bz->addData(packet.time, packet.sensorMag1.at(2));

    sensorGraph2.bx->addData(packet.time, packet.sensorMag2.at(0));
    sensorGraph2.by->addData(packet.time, packet.sensorMag2.at(1));
    sensorGraph2.bz->addData(packet.time, packet.sensorMag2.at(2));

    ui->sensorPlot->replot();
}

/************************************
 * Other
 **************************************/
void MainWindow::connStatusChanged(bool connected)
{
    QString statusColor = connected ? "green" : "red";
    ui->connStatus->setStyleSheet( QString("background : %1; color : white").arg(statusColor) );

    QString text = connected ? "Connected" : "Not Connected";
    ui->connStatus->setText(text);
}

void MainWindow::updateBatteryLevel(double batteryLevelVal)
{
    ui->batteryLevel->setValue(batteryLevelVal);
}

/************************************
 * Closing Procedure
 **************************************/
void MainWindow::closeEvent(QCloseEvent *event) {
    pktThread.quit();
    pktThread.wait();
    event->accept();
}



MainWindow::~MainWindow()
{
    delete ui;

    pktThread.quit();
    pktThread.wait();
}


