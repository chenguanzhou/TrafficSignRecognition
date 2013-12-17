#include "mlpdialog.h"
#include "ui_mlpdialog.h"
#include <QSettings>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include "SignRecognitionToolkit.h"

MLPDialog::MLPDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MLPDialog)
{
    ui->setupUi(this);
    InitUI();
}

MLPDialog::~MLPDialog()
{
    delete ui;
}

void MLPDialog::on_comboBoxTrainingMethod_currentIndexChanged(int index)
{
    if (index==0)
    {
        ui->label_param1->setText("bp_dw_scale");
        ui->label_param2->setText("bp_moment_scale");
    }
    else
    {
        ui->label_param1->setText("rp_dw0");
        ui->label_param2->setText("rp_dw_min");
    }
}

void MLPDialog::on_buttonBox_accepted()
{
    cv::ANN_MLP_TrainParams params = GetMLPParam();
    cv::NeuralNet_MLP classifier;
    int layers = ui->spinBoxLayers->value();
    int eachLayerCount = ui->spinBoxEachLayerCounts->value();

    std::vector<int> layersize;
    layersize.push_back(63);
    layersize.insert(layersize.end(),layers,eachLayerCount);
    layersize.push_back(ClassCount);
    cv::Mat layerSizes(1,layersize.size(),CV_32SC1,&layersize[0]);
    std::cout<<layerSizes<<std::endl;
    classifier.create(layerSizes,ui->comboBoxActivationFunction->currentIndex());


    cv::Mat samples;
    cv::Mat response;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Classifier");
    QString mlpFile = setting.value("MLPClassifier").toString();
    setting.endGroup();

    setting.beginGroup("TrainDataConfig");
    for (int i=0;i<ClassCount;++i)
    {
        QStringList stringList=setting.value(QString("Class")+QString::number(i)+"_Files").toStringList();
        QString configPath = setting.value(QString("Class")+QString::number(i)+"_Config").toString();
        std::vector<cv::Mat> some_samples= SignRecognitionToolkit::GetTrainImageCrops(stringList,configPath);
        for (std::vector<cv::Mat>::iterator iter = some_samples.begin();iter!=some_samples.end();++iter)
        {
            samples.push_back(SignRecognitionToolkit::GetCropFeature(*iter,SignRecognitionToolkit::PAPER_63));
        }
        cv::Mat res(some_samples.size(),ClassCount,CV_32FC1);
        for (int k=0;k<some_samples.size();++k)
        {
            float *p = res.ptr<float>(k);
            for (int m=0;m<ClassCount;++m)
            {
                p[m] = m==i?1:0;
            }
        }
        response.push_back(res);
    }
    setting.endGroup();

    classifier.train(samples,response,cv::Mat(),cv::Mat(),params);
    classifier.save(mlpFile.toStdString().c_str(),"mlp");
    QMessageBox::information(this,tr("Completed!"),tr("Completed mlp training!"));
}

void MLPDialog::InitUI()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    cv::NeuralNet_MLP classifier;
    setting.beginGroup("Classifier");
    QString mlpFile = setting.value("MLPClassifier").toString();
    setting.endGroup();
    if (mlpFile.isEmpty())
    {
        cv::Mat layerSizes=(cv::Mat_<int>(1,3) << 63,5,3);
        classifier.create(layerSizes);
    }
    else
        classifier.load(mlpFile.toStdString().c_str(),"mlp");

    setting.beginGroup("TrainDataConfig");
    bool isValid = false;
    ClassCount = setting.value("Count").toInt(&isValid);
    if (isValid==false || ClassCount <= 0)
    {
        QMessageBox::critical(this,tr("Train failed!"),tr("Training Data hasn't been set!"));
        return;
    }

    ui->lineEditInputLayerCount->setText(QString::number(63));
    ui->lineEditOutputLayerCount->setText(QString::number(ClassCount));
}

cv::ANN_MLP_TrainParams MLPDialog::GetMLPParam()
{
    cv::ANN_MLP_TrainParams param;
    param.train_method = ui->comboBoxTrainingMethod->currentIndex();
    int term_crit = 0;
    if (ui->checkBoxMax_iter->isChecked())
        term_crit += CV_TERMCRIT_ITER;
    if (ui->checkBoxEpsilon->isChecked())
        term_crit += CV_TERMCRIT_EPS;
    param.term_crit = cvTermCriteria( term_crit ,ui->lineEditMax_iter->text().toDouble(),ui->lineEditEpsilon->text().toDouble());

    if (term_crit == cv::ANN_MLP_TrainParams::RPROP)
    {
        param.rp_dw0 = ui->lineEditParam1->text().toDouble();
        param.rp_dw_min = ui->lineEditParam2->text().toDouble();
    }
    else
    {
        param.bp_dw_scale = ui->lineEditParam1->text().toDouble();
        param.bp_moment_scale = ui->lineEditParam2->text().toDouble();
    }
    return param;
}
cv::NeuralNet_MLP MLPDialog::GetMLPClassifier()
{

}
