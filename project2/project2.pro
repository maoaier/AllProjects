#-------------------------------------------------
#
# Project created by QtCreator 2018-06-25T14:22:31
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = project2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ipaddredit.cpp \
    ippartlineedit.cpp \
    qftp.cpp \
    qurlinfo.cpp

HEADERS  += mainwindow.h \
    ipaddredit.h \
    ippartlineedit.h \
    qurlinfo.h \
    qftp.h

FORMS    += mainwindow.ui

RESOURCES += \
    prcture.qrc


 LIBS+="F:/QT/QT/5.5/mingw492_32/bin/Qt5Ftp.dll"

DISTFILES +=
