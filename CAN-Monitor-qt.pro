#-------------------------------------------------
#
# Project created by QtCreator 2018-06-09T14:02:49
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CAN-Monitor-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    treemodel.cpp \
    treenode.cpp \
    headertreenode.cpp \
    messagetreenode.cpp \
    CanAdapter.cpp \
    CanAdapterLawicel.cpp \
    lawicel.c \
    CanTreeModel.cpp \
    SerialPortComboBox.cpp

HEADERS  += mainwindow.h \
    treemodel.h \
    treenode.h \
    headertreenode.h \
    messagetreenode.h \
    CanAdapter.h \
    can_message.h \
    CanAdapterLawicel.h \
    lawicel.h \
    CanTreeModel.h \
    SerialPortComboBox.h

FORMS    += mainwindow.ui

DISTFILES += \
    ../Downloads/favicon.ico \
    favicon.ico