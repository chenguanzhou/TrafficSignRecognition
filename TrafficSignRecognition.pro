#-------------------------------------------------
#
# Project created by QtCreator 2013-12-07T15:53:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrafficSignRecognition
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingdialog.cpp

HEADERS  += mainwindow.h \
    settingdialog.h

FORMS    += mainwindow.ui \
    settingdialog.ui

LIBS += -lopencv_core247 -lopencv_imgproc247  -lopencv_highgui247  -lopencv_ml247

RESOURCES += \
    resource.qrc
