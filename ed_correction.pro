#-------------------------------------------------
#
# Project created by QtCreator 2017-01-31T16:29:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ed_correction
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    cFreqAnalysis.cpp \
    HelpFunc.cpp \
    ImageLoader.cpp \
    Resizer.cpp \
    IntensityStretch.cpp

HEADERS  += MainWindow.h \
    cFreqAnalysis.h \
    FreeImage.h \
    HelpFunc.h \
    ImageLoader.h \
    Resizer.h \
    Vectors.h \
    IntensityStretch.h

FORMS    += MainWindow.ui


LIBS += FreeImage.lib
