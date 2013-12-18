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
    SignRecognitionToolkit.cpp \
    mlpdialog.cpp \
    languagedialog.cpp

HEADERS  += mainwindow.h \
    settingdialog.h \
    traindialog.h \
    traintab.h \
    SignRecognitionToolkit.h \
    mlpdialog.h \
    languagedialog.h

FORMS    += mainwindow.ui \
    settingdialog.ui \
    traindialog.ui \
    traintab.ui \
    mlpdialog.ui \
    languagedialog.ui

win32:CONFIG(release, debug|release): LIBS += -lopencv_core247 -lopencv_imgproc247  -lopencv_highgui247  -lopencv_ml247
else:win32:CONFIG(debug, debug|release): LIBS += -lopencv_core247d -lopencv_imgproc247d  -lopencv_highgui247d  -lopencv_ml247d

unix: LIBS += -lopencv_core -lopencv_imgproc  -lopencv_highgui  -lopencv_ml

RESOURCES += \
    resource.qrc

TRANSLATIONS += zh_cn.ts us_en.ts

