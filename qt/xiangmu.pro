#-------------------------------------------------
#
# Project created by QtCreator 2016-01-09T14:35:48
#
#-------------------------------------------------

QT       += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xiangmu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    messagewindow.cpp \
    lookwindow.cpp \
    specificationwindow.cpp

HEADERS  += mainwindow.h \
    messagewindow.h \
    lookwindow.h \
    specificationwindow.h

FORMS    += mainwindow.ui \
    messagewindow.ui \
    lookwindow.ui \
    specificationwindow.ui
