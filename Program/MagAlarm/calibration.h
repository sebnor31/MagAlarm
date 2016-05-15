#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "typedef.h"

class Calibration : public QObject
{
    Q_OBJECT

private:
    QFile rawMagFile;
    QTextStream out;

public:
    explicit Calibration(QString rawMagFilePath, QObject *parent = 0);

public slots:
    void saveRawMagField(DataPacket packet);
    void stopSaving();
};

#endif // CALIBRATION_H
