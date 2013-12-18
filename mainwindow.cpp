#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QLineEdit>
#include "SignRecognitionToolkit.h"
#include "traindialog.h"
#include "mlpdialog.h"
#include "languagedialog.h"
#include <opencv2/opencv.hpp>

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
    ui->graphicsView->fitInView((QGraphicsItem *)item,Qt::KeepAspectRatio);
    lineEditCurrentID->setText(QString().number(imageListID+1)+"/"+QString().number(imageFileList.size()));


    static int hehe = 0;
    hehe++;
    if (ui->actionMLPDynamicTraining->isChecked() && hehe%10==0)
    {
        cv::NeuralNet_MLP classifier;
        QSettings setting("config.ini",QSettings::IniFormat);
        setting.beginGroup("Classifier");
        QString mlpFile = setting.value("MLPClassifier").toString();
        setting.endGroup();
        if (!mlpFile.isEmpty())
            classifier.load(mlpFile.toStdString().c_str(),"mlp");

        cv::Mat src = cv::imread(QDir::fromNativeSeparators(imageFileList.at(imageListID).filePath()).toStdString());
        std::vector<cv::Mat> crops;
        SignRecognitionToolkit::GetTestImageCrop(src,crops);

        ui->listWidget->clear();

        for (int i=0;i<crops.size();++i)
        {
            cv::Mat feature = SignRecognitionToolkit::GetCropFeature(crops[i],SignRecognitionToolkit::PAPER_63);
            uchar* p =crops[i].ptr<uchar>(0);
            cv::Mat result;
            classifier.predict(feature,result);
            float* res = result.ptr<float>(0);
            int nFlags = std::max_element(res,res+result.cols)-res;
            if (res[nFlags]<0.7)
                continue;

            cv::imwrite("temp.bmp",crops[i]);
            ui->listWidget->addItem(new QListWidgetItem(QIcon("temp.bmp"),tr("The ")+QString::number(nFlags+1)+tr("th class")));
        }
    }


    ++imageListID;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    if (graphicsScene->items().size() != 0)
        ui->graphicsView->fitInView(graphicsScene->items()[0],Qt::KeepAspectRatio);
}

void MainWindow::SetEnableNavigateActions(bool enabled)
{
    ui->actionFirstImage->setEnabled(enabled);
    ui->actionLastImage->setEnabled(enabled);
    ui->actionNextImage->setEnabled(enabled);
    ui->actionPreviousImage->setEnabled(enabled);
    ui->actionAnimation->setEnabled(enabled);
}

void MainWindow::SetSingleImageActions(bool enabled)
{
    ui->actionSVMTestingForSingleImage->setEnabled(enabled);
    ui->actionMLPTestingForSingleImage->setEnabled(enabled);
}

void MainWindow::on_actionSetTrainData_triggered()
{
    TrainDialog *dlg = new TrainDialog(true);
    dlg->setModal(true);
    dlg->exec();
    delete dlg;
}

void MainWindow::on_actionSetTestData_triggered()
{
    TrainDialog *dlg = new TrainDialog(false);
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
    SetEnableNavigateActions(true);
    SetSingleImageActions(false);
    lineEditCurrentID->setText(QString());
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
        cv::Mat res(some_samples.size(),1,CV_32SC1);
        for (int k=0;k<some_samples.size();++k)
        {
            * (res.ptr<int>(k))=i;
        }
        response.push_back(res);
    }
    setting.endGroup();

//    cv::imshow("hehe",samples);
//    cv::waitKey();

    classifier.train(samples,response);
    classifier.save(svmFile.toStdString().c_str(),"svm");
    QMessageBox::information(this,tr("Completed!"),tr("Completed svm training!"));
}

void MainWindow::on_actionOpenSingleImage_triggered()
{
    singleImagePath = QFileDialog::getOpenFileName(this,tr("Open an image file"),singleImagePath,"*.bmp *.jpg *.ppm *.png");
    singleImagePath = QDir::fromNativeSeparators(singleImagePath);
    if (!singleImagePath.isEmpty())
    {
        QPixmap image(singleImagePath);
//        cv::Mat src = cv::imread(QDir::fromNativeSeparators(singleImagePath));
        if (image.isNull() == true)
        {
            QMessageBox::warning(this,tr("Error"),tr("Open image file ")+singleImagePath+tr(" failed"));
            return ;
        }
        graphicsScene->clear();
        QGraphicsPixmapItem *item= new QGraphicsPixmapItem( /*QPixmap::fromImage(*/image/*)*/);
        graphicsScene->addItem(item);
        ui->graphicsView->fitInView(item,Qt::KeepAspectRatio);
        SetEnableNavigateActions(false);
        SetSingleImageActions(true);
    }
}

void MainWindow::on_actionSVMTestingForSingleImage_triggered()
{
    cv::SVM classifier;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Classifier");
    QString svmFile = setting.value("SVMClassifier").toString();
    setting.endGroup();
    if (!svmFile.isEmpty())
        classifier.load(svmFile.toStdString().c_str(),"svm");

    cv::Mat src = cv::imread(singleImagePath.toStdString());
    std::vector<cv::Mat> crops;
    SignRecognitionToolkit::GetTestImageCrop(src,crops);
    qDebug()<<"crops.size():"<<crops.size();
    for (int i=0;i<crops.size();++i)
    {
        cv::Mat feature = SignRecognitionToolkit::GetCropFeature(crops[i],SignRecognitionToolkit::PAPER_63);
        uchar* p =crops[i].ptr<uchar>(0);

        int nFlag = classifier.predict(feature);
        QMessageBox::information(this,tr("result"),tr("This belongs to the ")+QString::number(nFlag+1)+tr("th class!"));
    }
}



void MainWindow::on_actionMLPTraining_triggered()
{
//    cv::NeuralNet_MLP classifier;
//    QSettings setting("config.ini",QSettings::IniFormat);
//    setting.beginGroup("Classifier");
//    QString mlpFile = setting.value("MLPClassifier").toString();
//    setting.endGroup();
//    if (mlpFile.isEmpty())
//    {
//        cv::Mat layerSizes=(cv::Mat_<int>(1,3) << 63,5,3);
//        classifier.create(layerSizes);
//    }
//    else
//    {
//        classifier.load(mlpFile.toStdString().c_str(),"mlp");
//    }

//    setting.beginGroup("TrainDataConfig");
//    bool isValid = false;
//    int ClassCount = setting.value("Count").toInt(&isValid);
//    if (isValid==false || ClassCount <= 0)
//    {
//        QMessageBox::critical(this,tr("Train failed!"),tr("Training Data hasn't been set!"));
//        return;
//    }

    MLPDialog *dlg = new MLPDialog();
    dlg->show();
//    cv::NeuralNet_MLP classifier = dlg.GetMLPClassifier();
//    cv::ANN_MLP_TrainParams params = dlg.GetMLPParam();

//    cv::Mat samples;
//    cv::Mat response;
//    for (int i=0;i<ClassCount;++i)
//    {
//        QStringList stringList=setting.value(QString("Class")+QString::number(i)+"_Files").toStringList();
//        QString configPath = setting.value(QString("Class")+QString::number(i)+"_Config").toString();
//        std::vector<cv::Mat> some_samples= SignRecognitionToolkit::GetTrainImageCrops(stringList,configPath);
//        for (std::vector<cv::Mat>::iterator iter = some_samples.begin();iter!=some_samples.end();++iter)
//        {
//            samples.push_back(SignRecognitionToolkit::GetCropFeature(*iter,SignRecognitionToolkit::PAPER_63));
//        }
//        cv::Mat res(some_samples.size(),ClassCount,CV_32FC1);
//        for (int k=0;k<some_samples.size();++k)
//        {
//            float *p = res.ptr<float>(k);
//            for (int m=0;m<ClassCount;++m)
//            {
//                p[m] = m==i?1:0;
//            }
//        }
//        response.push_back(res);
//    }
//    setting.endGroup();

////    cv::imshow("hehe",samples);
////    cv::waitKey();

//    classifier.train(samples,response,cv::Mat(),cv::Mat(),params);
////    classifier.save(mlpFile.toStdString().c_str(),"mlp");
//    QMessageBox::information(this,tr("Completed!"),tr("Completed mlp training!"));
}

void MainWindow::on_actionMLPTestingForSingleImage_triggered()
{
    cv::NeuralNet_MLP classifier;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Classifier");
    QString mlpFile = setting.value("MLPClassifier").toString();
    setting.endGroup();
    if (!mlpFile.isEmpty())
        classifier.load(mlpFile.toStdString().c_str(),"mlp");

    cv::Mat src = cv::imread(singleImagePath.toStdString());
    std::vector<cv::Mat> crops;
    SignRecognitionToolkit::GetTestImageCrop(src,crops);

    for (int i=0;i<crops.size();++i)
    {
        cv::Mat feature = SignRecognitionToolkit::GetCropFeature(crops[i],SignRecognitionToolkit::PAPER_63);
        uchar* p =crops[i].ptr<uchar>(0);
        cv::Mat result;
        classifier.predict(feature,result);
        float* res = result.ptr<float>(0);
        int nFlags = std::max_element(res,res+result.cols)-res;
        QMessageBox::information(this,tr("result"),tr("This belongs to the ")+QString::number(nFlags+1)+tr("th class!"));
    }
}

void MainWindow::on_actionMLPTestingForTestData_triggered()
{
    cv::NeuralNet_MLP classifier;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Classifier");
    QString mlpFile = setting.value("MLPClassifier").toString();
    setting.endGroup();
    if (!mlpFile.isEmpty())
        classifier.load(mlpFile.toStdString().c_str(),"mlp");


    setting.beginGroup("TestDataConfig");
    int ClassCount = setting.value("Count").toInt();
    if (ClassCount == 0)  return;


    int nTotal = 0;
    int nCorect = 0;
    for (int i=0;i<ClassCount;++i)
    {
        QStringList stringList=setting.value(QString("Class")+QString::number(i)+"_Files").toStringList();
        QString configPath = setting.value(QString("Class")+QString::number(i)+"_Config").toString();
        std::vector<cv::Mat> testobject= SignRecognitionToolkit::GetTrainImageCrops(stringList,configPath);
        for (std::vector<cv::Mat>::iterator iter = testobject.begin();iter!=testobject.end();++iter)
        {
            cv::Mat result;
            cv::Mat feature = SignRecognitionToolkit::GetCropFeature(*iter,SignRecognitionToolkit::PAPER_63);
            classifier.predict(feature,result);
            float* res = result.ptr<float>(0);
            int nFlags = std::max_element(res,res+result.cols)-res;

            if (nFlags == i)
                ++nCorect;
            ++nTotal;
        }
    }
    setting.endGroup();

    QMessageBox::information(this,tr("result"),QString("correct: ")+QString::number(nCorect)+"/"+QString::number(nTotal)+"="+QString::number(nCorect*100/nTotal)+"%");
}

void MainWindow::on_actionLanguage_triggered()
{
    LanguageDialog* dlg = new LanguageDialog();
    dlg->show();
}

