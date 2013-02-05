#-------------------------------------------------
#
# Project created by QtCreator 2013-01-31T22:44:46
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = cpptestapp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

QMAKE_CXXFLAGS += -std=c++0x

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += botan-1.10
}

HEADERS += \
    serializer.h
