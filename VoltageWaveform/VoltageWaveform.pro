#-------------------------------------------------
#
# Project created by QtCreator 2018-09-29T16:43:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = VoltageWaveform
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    fftoperation.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    fftw3.h \
    fftoperation.h

FORMS    += mainwindow.ui

LIBS += -LF:/Linux/GIT/AllProjects/VoltageWaveform -lfftw3f-3
#LIBS += -LF:\Linux\GIT\AllProjects\VoltageWaveform\ -lfftw3f-3
#LIBS += "F:\Linux\GIT\AllProjects\VoltageWaveform\libfftw3f-3.lib"

