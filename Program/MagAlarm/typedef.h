#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <QObject>
#include <QVector>

struct DataPacket {
    uchar counter;
    ushort battery;
    QVector<ushort> refMag;
    QVector<ushort> sensorMag1;
    QVector<ushort> sensorMag2;
};
const int dontcare = qRegisterMetaType<DataPacket>("DataPacket");

#endif // TYPEDEF_H
