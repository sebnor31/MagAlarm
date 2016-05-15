#include "calibration.h"
#include <QFileDialog>

Calibration::Calibration(QString rawMagFilePath, QObject *parent) : QObject(parent)
{
    // Open the file where uncalibrated (raw) magnetic fields will be saved
    rawMagFile.setFileName(rawMagFilePath);
    out.setDevice(&rawMagFile);
    rawMagFile.open(QIODevice::WriteOnly | QIODevice::Text);
}

void Calibration::saveRawMagField(DataPacket packet)
{
    foreach (ushort magVal , packet.refMag) {
        out << magVal << " ";
    }

    foreach (ushort magVal , packet.sensorMag1) {
        out << magVal << " ";
    }

    for (int i = 0; i < packet.sensorMag2.size(); i++) {

        out << packet.sensorMag2.at(i);

        // Manage the last suffix to be end of line
        bool lastValue = (i == (packet.sensorMag2.size() - 1));
        QString suffix = lastValue ? "\n" : " ";
        out << suffix;
    }
}

void Calibration::stopSaving()
{
    rawMagFile.close();
}
