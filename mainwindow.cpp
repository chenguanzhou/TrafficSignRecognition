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
#include "SignRecognitionToolkit.h"
#include "traindialog.h"

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

void MainWindow::on_actionTestCanny_triggered()
{
    SignRecognitionToolkit hehe;
    cv::Mat testImage= cv::imread("D://image.035350.jpg");
    std::vector<cv::Mat> vec;
    hehe.GetTestImageCrop(testImage,vec);
    hehe.GetCropFeature(vec[2],SignRecognitionToolkit::PAPER_63);
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



void MainWindow::on_actionSetTrainData_triggered()
{
    TrainDialog *dlg = new TrainDialog();
    dlg->setModal(true);
    dlg->exec();
    delete dlg;
}

void MainWindow::on_actionBrowseTestSet_triggered()//浏览TestSet
{
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Directory");
    QString imageDirString = setting.value("TestSetDir").toString();
    setting.endGroup();
    if (imageDirString.size()==0)
    {
        if (QMessageBox::question(this,tr("No Directory Infomation"),tr("The diarectory of test set hasn't been set! Edit settings now ?"))==QMessageBox::Yes)
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
    ui->actionFirstImage->setEnabled(true);
    ui->actionLastImage->setEnabled(true);
    ui->actionNextImage->setEnabled(true);
    ui->actionPreviousImage->setEnabled(true);
    ui->actionAnimation->setEnabled(true);
    ShowCurrentImage();
}

void MainWindow::on_actionSVMTraining_triggered()
{
    cv::SVM classifier;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Classifier");
    QString svmFile = setting.value("SVMClassifier").toString();
    setting.endGroup();
    if (!svmFile.isEmpty())
        classifier.load(svmFile.toStdString().c_str(),"svm");

    setting.beginGroup("TrainDataConfig");
    bool isValid = false;
    int ClassCount = setting.value("Count").toInt(&isValid);
    if (isValid==false || ClassCount <= 0)
    {
        QMessageBox::critical(this,tr("Train failed!"),tr("Training Data hasn't been set!"));
        return;
    }

    cv::Mat samples;
    cv::Mat response;
    for (int i=0;i<ClassCount;++i)
    {
        QStringList stringList=setting.value(QString("Class")+QString::number(i)+"_Files").toStringList();
        QString configPath = setting.value(QString("Class")+QString::number(i)+"_Config").toString();
        std::vector<cv::Mat> some_samples= SignRecognitionToolkit::GetTrainImageCrops(stringList,configPath);
        for (std::vector<cv::Mat>::iterator iter = some_samples.begin();iter!=some_samples.end();++iter)
        {
            samples.push_back(SignRecognitionToolkit::GetCropFeature(*iter,SignRecognitionToolkit::PAPER_63));
        }
        cv::Mat res(some_samples.size(),1,CV_32SC1,cv::Scalar(i));
        response.push_back(res);
    }
    setting.endGroup();

    classifier.train(samples,response);
    classifier.save(svmFile.toStdString().c_str(),"svm");
}
