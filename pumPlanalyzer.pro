#-------------------------------------------------
#
# Project created by QtCreator 2016-10-12T12:10:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pumPlanalyzer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fileoperatinos.cpp \
    datastructure.cpp

HEADERS  += mainwindow.h \
    fileoperatinos.h \
    datastructure.h

FORMS    += mainwindow.ui

CONFIG+= static
