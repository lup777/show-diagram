#-------------------------------------------------
#
# Project created by QtCreator 2016-09-30T13:17:37
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test4
TEMPLATE = app
CONFIG += console


SOURCES += main.cpp\
        mainwindow.cpp \
    WorkThread.cpp \
    screen.cpp \
    DataFile.cpp

HEADERS  += mainwindow.h \
    WorkThread.h \
    screen.h \
    DataFile.h

FORMS    += mainwindow.ui
