#ifndef PACKETREADER_H
#define PACKETREADER_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
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
    QTimer *timer;

    uchar readPacketRequest[6] = { 170, 170, 15, 255, 15, 255 };

    const int RX_BUFFSIZE = 22;
    char bufferdata[22];

signals:
    void newPacket(QVector<ushort> packetData);

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
    void processPacket(QVector<ushort> rawPacket);

signals:
    void batteryLevel(double);
    void doorStatus(int);
};



#endif // PACKETREADER_H
