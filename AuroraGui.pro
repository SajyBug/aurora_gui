#-------------------------------------------------
#
# Project created by QtCreator 2016-12-03T20:38:22
#
#-------------------------------------------------

QT       += core gui network xml

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AuroraGui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connector.cpp \
    UICmdResponse.cpp

HEADERS  += mainwindow.h \
    connector.h \
    UICmdResponse.h

FORMS    += mainwindow.ui

DISTFILES += \
    no_tile_loaded.jpg
