#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <QObject>
#include <QVector>

struct DataPacket {
    double time;
    uchar counter;
    ushort battery;
    QVector<ushort> refMag;
    QVector<ushort> sensorMag1;
    QVector<ushort> sensorMag2;
};
const int dontcare = qRegisterMetaType<DataPacket>("DataPacket");

const int dontcare2 = qRegisterMetaType< QVector<ushort> >("QVector<ushort>");

#endif // TYPEDEF_H
