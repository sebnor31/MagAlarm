#-------------------------------------------------
#
# Project created by QtCreator 2016-04-30T20:01:38
#
#-------------------------------------------------


TARGET      = MagAlarm
VERSION     = 1.0
TEMPLATE    = app


QT       += core gui serialport widgets printsupport

SOURCES += main.cpp\
        mainwindow.cpp \
    packetreader.cpp \
    serialib.cpp \
    qcustomplot.cpp \
    calibration.cpp

HEADERS  += mainwindow.h \
    packetreader.h \
    serialib.h \
    qcustomplot.h \
    typedef.h \
    calibration.h

FORMS    += mainwindow.ui
