#-------------------------------------------------
#
# Project created by QtCreator 2013-12-07T15:53:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrafficSignRecognition
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    settingdialog.cpp \
    traindialog.cpp \
    traintab.cpp \
    SignRecognitionToolkit.cpp

HEADERS  += mainwindow.h \
    settingdialog.h \
    traindialog.h \
    traintab.h \
    SignRecognitionToolkit.h

FORMS    += mainwindow.ui \
    settingdialog.ui \
    traindialog.ui \
    traintab.ui

win32:LIBS += -lopencv_core247 -lopencv_imgproc247  -lopencv_highgui247  -lopencv_ml247
unix: LIBS += -lopencv_core -lopencv_imgproc  -lopencv_highgui  -lopencv_ml

RESOURCES += \
    resource.qrc


