#include "packetreader.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QByteArray>

PacketReader::PacketReader(QObject *parent) : QObject(parent)
{


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
    timer->start(readPacketDelay);
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

    qDebug() << QString("PASS: Receiver is CONNECTED");
}

void PacketReader::readPacket()
{
    ushort bufferdataOut[15] = { 0 };

    rxPort->Write(readPacketRequest, 6);

    rxPort->Read(bufferdata, RX_BUFFSIZE);

    QString packetVals = "[ ";

    for (int x = 0; x < 9; x++)
    {
        bufferdataOut[x] = ((ushort)(bufferdata[x * 2 + 4] * 256) + (ushort)bufferdata[x * 2 + 5]);

        packetVals += QString::number(bufferdataOut[x]) + ",";
    }

    qDebug() << packetVals;



//    emit newPacket(array);
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
    connect(packetReader, SIGNAL(newPacket(QByteArray)), this, SLOT(processPacket(QByteArray)));

    pktReaderThread.start();
}


void PacketManager::processPacket(QByteArray rawPacket)
{
    qDebug() << "New Packet: " << rawPacket;
}

PacketManager::~PacketManager()
{
    pktReaderThread.quit();
    pktReaderThread.wait();
    qDebug() << "-- Packet Manager Destroyed --";
}
