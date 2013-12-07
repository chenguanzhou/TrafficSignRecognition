#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QHBoxLayout>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imageListID(0)
{
    ui->setupUi(this);
    graphicsScene = new QGraphicsScene();
    ui->graphicsView->setScene(graphicsScene);
    lineEditCurrentID = new QLineEdit();
    lineEditCurrentID->setMaximumWidth(80);
    lineEditCurrentID->setAlignment(Qt::AlignCenter);
    lineEditCurrentID->setReadOnly(true);
    ui->navigatorToolBar->insertWidget(ui->actionNextImage, lineEditCurrentID);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_animate()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    delete graphicsScene;
    delete lineEditCurrentID;
}


void MainWindow::on_actionSettings_triggered()
{
    SettingDialog *dlg = new SettingDialog();
    dlg->setModal(true);
    dlg->exec();
    delete dlg;
}

void MainWindow::on_actionTrainSet_triggered()//浏览
{
    QString imageDirString;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Directory");
    imageDirString = setting.value("TrainSetDir").toString();
    setting.endGroup();
    if (imageDirString.size()==0)
    {
        if (QMessageBox::question(this,tr("No Directory Infomation"),tr("The diarectory of train set hasn't been set! Edit settings now ?"))==QMessageBox::Yes)
            ui->actionSettings->trigger();
        return;
    }

    QDir dir(imageDirString);
    if (dir.exists() == false)
    {
        QMessageBox::critical(this,tr("Error!"),tr("The Directory ")+imageDirString+tr(" is not existed!"));
        return;
    }

    QStringList filter;
    filter.push_back("*.bmp");
    filter.push_back("*.jpg");
    filter.push_back("*.png");
    filter.push_back("*.ppm");
    imageFileList.clear();
    imageFileList = QList<QFileInfo>(dir.entryInfoList(filter));
    if (imageFileList.size()==0)
    {
        QMessageBox::critical(this,tr("Error!"),tr("The Directory ")+imageDirString+tr(" is empty!"));
        return;
    }
    imageListID = 0;
    ui->navigatorToolBar->setEnabled(true);
    ShowCurrentImage();
}

void MainWindow::on_actionNextImage_triggered()
{
    ShowCurrentImage();
}

void MainWindow::on_actionPreviousImage_triggered()
{
    imageListID -= 2;
    ShowCurrentImage();
}

void MainWindow::on_animate()
{
    ShowCurrentImage();
}

void MainWindow::ShowCurrentImage()
{
    if (imageFileList.size() == 0)
        return;
    if (imageListID == imageFileList.size())
        imageListID = 0;
    else if (imageListID == -1)
        imageListID = imageFileList.size()-1;
    graphicsScene->clear();
    QImage image(imageFileList.at(imageListID).filePath());
    QGraphicsPixmapItem *item= new QGraphicsPixmapItem( QPixmap::fromImage(image));
    graphicsScene->addItem(item);
    ui->graphicsView->fitInView(QRectF(0, 0, graphicsScene->width(), graphicsScene->height()));
    lineEditCurrentID->setText(QString().number(imageListID+1)+"/"+QString().number(imageFileList.size()));
    ++imageListID;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   ui->graphicsView->fitInView(QRectF(0, 0, graphicsScene->width(), graphicsScene->height()));
}


void MainWindow::on_actionAnimation_toggled(bool arg1)
{
    if (arg1==true)
    {
        timer->start(50);
        ui->actionAnimation->setToolTip(tr("Pause"));
    }
    else
    {
        timer->stop();
        ui->actionAnimation->setToolTip(tr("Play Animation"));
    }
}

void MainWindow::on_actionFirstImage_triggered()
{
    imageListID = 0;
    ShowCurrentImage();
}

void MainWindow::on_actionLastImage_triggered()
{
    imageListID = imageFileList.size()-1;
    ShowCurrentImage();
}
