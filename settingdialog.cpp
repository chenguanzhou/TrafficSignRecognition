#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QFileDialog>
#include <QSettings>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    ReadParams();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::ReadParams()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Directory");
    {
        ui->lineEditTrainSet->setText(setting.value("TrainSetDir").toString());
        ui->lineEditTestSet->setText(setting.value("TestSetDir").toString());
    }
    setting.endGroup();
    setting.beginGroup("Classifier");
    {
        QString path = setting.value("SVMClassifier").toString();
        path = path.isEmpty()?QApplication::instance()->applicationDirPath()+"\\svm_classifier.xml":path;
        ui->lineEditSVM->setText(path);
        path = setting.value("MLPClassifier").toString();
        path = path.isEmpty()?QApplication::instance()->applicationDirPath()+"\\mlp_classifier.xml":path;
        ui->lineEditMLP->setText(path);
    }
    setting.endGroup();
}

void SettingDialog::WriteParams()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Directory");
    {
        setting.setValue("TrainSetDir",ui->lineEditTrainSet->text());
        setting.setValue("TestSetDir",ui->lineEditTestSet->text());
    }
    setting.endGroup();
    setting.beginGroup("Classifier");
    {
        setting.setValue("SVMClassifier",ui->lineEditSVM->text());
        setting.setValue("MLPClassifier",ui->lineEditMLP->text());
    }
    setting.endGroup();
}

void SettingDialog::on_pushButtonTrainSetBrowse_clicked()
{
    QString TrainSetDir = QFileDialog::getExistingDirectory(
                this,tr("Directory of Train Data Set"),
                ui->lineEditTrainSet->text());
    if (!TrainSetDir.isEmpty())
        ui->lineEditTrainSet->setText(TrainSetDir);
}

void SettingDialog::on_pushButtonTestSetBrowse_clicked()
{
    QString TestSetDir = QFileDialog::getExistingDirectory(
                this,tr("Directory of Test Data Set"),
                ui->lineEditTestSet->text());
    if (!TestSetDir.isEmpty())
        ui->lineEditTestSet->setText(TestSetDir);
}

void SettingDialog::on_buttonBox_accepted()
{
    WriteParams();
}

void SettingDialog::on_pushButtonTestSetBrowseSVM_clicked()
{
    QString SVMDir = QFileDialog::getSaveFileName(this,tr("SVM Classifier file"),QApplication::instance()->applicationDirPath(),"*.xml");
    if (!SVMDir.isEmpty())
        ui->lineEditSVM->setText(SVMDir);
}

void SettingDialog::on_pushButtonTestSetBrowseMLP_clicked()
{
    QString MLPDir = QFileDialog::getSaveFileName(this,tr("MLP Classifier file"),QApplication::instance()->applicationDirPath(),"*.xml");
    if (!MLPDir.isEmpty())
        ui->lineEditMLP->setText(MLPDir);
}
