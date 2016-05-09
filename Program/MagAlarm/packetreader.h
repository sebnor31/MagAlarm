#ifndef PACKETREADER_H
#define PACKETREADER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "serialib.h"
#include "typedef.h"


class PacketReader : public QObject
{
    Q_OBJECT

public:
    explicit PacketReader(QObject *parent = 0);
    ~PacketReader();

public slots:
    void start();

private:
    void connectToReceiver();

private slots:
    void readPacket();

private:
    serialib *rxPort;
    QTimer   *timer;

    // Message to initialize connection between transmitter and usb receiver
    uchar readPacketRequest[6] = { 170, 170, 15, 255, 15, 255 };

    // Data packet parameters
    const int RX_BUFFSIZE = 22;
    char bufferdata[22];

signals:
    void newPacket(DataPacket dataPacket);
};


class PacketManager : public QObject
{
    Q_OBJECT

public:
    explicit PacketManager(QObject *parent = 0);
    ~PacketManager();

private:
    QThread pktReaderThread;
    PacketReader *packetReader;

public slots:
    void start();
    void processPacket(DataPacket rawPacket);

signals:
    void batteryLevel(double);
    void doorStatus(int);
};



#endif // PACKETREADER_H
