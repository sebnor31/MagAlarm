#-------------------------------------------------
#
# Project created by QtCreator 2016-04-30T20:01:38
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MagAlarm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    packetreader.cpp \
    serialib.cpp

HEADERS  += mainwindow.h \
    packetreader.h \
    serialib.h

FORMS    += mainwindow.ui
