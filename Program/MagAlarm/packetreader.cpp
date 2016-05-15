#include "packetreader.h"
#include <QSerialPortInfo>
#include <QDebug>

/*********************************
 * Packet Reader (low level)
 *********************************/

PacketReader::PacketReader(QObject *parent) : QObject(parent)
{
    readPacketRequest[0] = 170;
    readPacketRequest[1] = 170;
    readPacketRequest[2] = 15;
    readPacketRequest[3] = 255;
    readPacketRequest[4] = 15;
    readPacketRequest[5] = 255;

}

void PacketReader::start()
{
    QList<QSerialPortInfo> serialPortList = QSerialPortInfo::availablePorts();

    bool rxPortFound = false;
    QSerialPortInfo serialRxInfo;

    foreach (QSerialPortInfo portInfo, serialPortList) {
        if (portInfo.manufacturer() == "FTDI") {
            serialRxInfo = portInfo;
            rxPortFound = true;
            break;
        }
    }

    if (!rxPortFound) {
        qDebug() << "ERROR: No Receiver Found!!";
        return;
    }

    qDebug() << "PASS: Receiver Found at " << serialRxInfo.portName();

    rxPort = new serialib();
    int isOpen = rxPort->Open(serialRxInfo.portName().toLatin1(), 921600);

    if (isOpen != 1) {
        qDebug() << "ERROR: Receiver could not be opened -- Code: " << isOpen;
        return;
    }

    connectToReceiver();

    const int readPacketDelay = 1000;     // Delay in msec
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readPacket()));
    connect(this, SIGNAL(samplingFreq(int)), timer, SLOT(start(int)));

    emit samplingFreq(readPacketDelay);
}

void PacketReader::updateSamplingFreq(int msec)
{
    emit samplingFreq(msec);
}

void PacketReader::connectToReceiver()
{
    uchar connectRequest[6] = { 204, 204, 3, 232, 1, 0};

    int numBytesRead = 0;
    char buffer[1] = {0};

    while (numBytesRead == 0) {
        rxPort->Write(connectRequest, 6);
        numBytesRead = rxPort->Read(buffer, 1);
    }

    emit connectedSig(true);
    qDebug() << QString("PASS: Receiver is CONNECTED");

    time.start();
}

void PacketReader::readPacket()
{
    DataPacket packet;

    // Read raw data from receiver
    rxPort->Write(readPacketRequest, 6);
    rxPort->Read(bufferdata, RX_BUFFSIZE);

    // Set time stamp
    packet.time = (time.elapsed() / 1000.0);

    // Get counter value
    packet.counter  = static_cast<uchar>(bufferdata[1]);

    // Get battery level
    packet.battery  = static_cast<ushort>( ( static_cast<uchar>(bufferdata[2]) * 256 ) + static_cast<uchar>(bufferdata[3]));

    // Get magnetic field values for reference sensor
    for (int x = 0; x < 3; x++)
    {
        ushort magAxis = ((ushort)(bufferdata[x * 2 + 4] * 256) + (ushort)bufferdata[x * 2 + 5]);
        packet.sensorMag2.append(magAxis);
    }

    // Get magnetic field values for sensor 1
    for (int x = 0; x < 3; x++)
    {
        ushort magAxis = ((ushort)(bufferdata[x * 2 + 10] * 256) + (ushort)bufferdata[x * 2 + 11]);
        packet.sensorMag1.append(magAxis);
    }

    // Get magnetic field values for sensor 2
    for (int x = 0; x < 3; x++)
    {
        ushort magAxis = ((ushort)(bufferdata[x * 2 + 16] * 256) + (ushort)bufferdata[x * 2 + 17]);
        packet.refMag.append(magAxis);
    }

    emit newPacket(packet);
}

PacketReader::~PacketReader()
{
    timer->stop();

    rxPort->Close();
    delete rxPort;

    qDebug() << "-- Receiver Port is closed --";
}

/*********************************
 * Packet Manager
 *********************************/
PacketManager::PacketManager(QObject *parent) : QObject(parent)
{}

void PacketManager::start()
{
    packetReader = new PacketReader();
    packetReader->moveToThread(&pktReaderThread);
    connect(&pktReaderThread, SIGNAL(started()), packetReader, SLOT(start()));
    connect(&pktReaderThread, SIGNAL(finished()), packetReader, SLOT(deleteLater()), Qt::DirectConnection);

    connect(packetReader, SIGNAL(connectedSig(bool)), this, SIGNAL(connectedSig(bool)));
    connect(packetReader, SIGNAL(newPacket(DataPacket)), this, SLOT(processPacket(DataPacket)));
    connect(this, SIGNAL(updateSamplingFreq(int)), packetReader, SLOT(updateSamplingFreq(int)), Qt::DirectConnection);

    pktReaderThread.start();
}

void PacketManager::processPacket(DataPacket rawPacket)
{
    // Convert battery absolute value to a percentage (ADC is done in 10 bits => 1024 numerical value)
    double batteryLevelVal = ( static_cast<double>(rawPacket.battery) / 1024.0 ) * 100.0;
    emit batteryLevel(batteryLevelVal);

//    emit refMagSig(rawPacket.time, rawPacket.refMag);
//    emit sensor1MagSig(rawPacket.time, rawPacket.sensorMag1);
//    emit sensor2MagSig(rawPacket.time, rawPacket.sensorMag2);

    emit magFieldData(rawPacket);

    //    qDebug() << QString("Counter = %1  -  Battery = %2").arg(rawPacket.counter).arg(batteryLevelVal);
}

void PacketManager::samplingFreq(int msec)
{
    emit updateSamplingFreq(msec);
}

PacketManager::~PacketManager()
{
    pktReaderThread.quit();
//    pktReaderThread.wait(1000);
    qDebug() << "-- Packet Manager Destroyed --";
}
