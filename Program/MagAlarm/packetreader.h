#ifndef PACKETREADER_H
#define PACKETREADER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QTime>
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
    void updateSamplingFreq(int msec);

private:
    void connectToReceiver();

private slots:
    void readPacket();

private:
    serialib *rxPort;
    QTimer   *timer;
    QTime    time;

    // Message to initialize connection between transmitter and usb receiver
    uchar readPacketRequest[6];

    // Data packet parameters
    const int RX_BUFFSIZE = 22;
    char bufferdata[22];

signals:
    void connectedSig(bool);
    void newPacket(DataPacket dataPacket);
    void samplingFreq(int msec);
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
    void samplingFreq(int msec);

signals:
    void connectedSig(bool);
    void batteryLevel(double);
    void doorStatus(int);
    void magFieldData(DataPacket packet);
    void refMagSig(double time, QVector<ushort> magField);
    void sensor1MagSig(double time, QVector<ushort> magField);
    void sensor2MagSig(double time, QVector<ushort> magField);
    void updateSamplingFreq(int msec);
};



#endif // PACKETREADER_H
