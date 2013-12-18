#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Language");
    QTranslator  translator;
    if(translator.load(setting.value("language").toString()))
        a.installTranslator(&translator);
    setting.endGroup();

    MainWindow w;
    w.show();

    return a.exec();
}
