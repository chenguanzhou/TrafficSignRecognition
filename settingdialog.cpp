#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QSettings>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    InitParams();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::InitParams()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Directory");
    {
        ui->lineEditTrainSet->setText(setting.value("TrainSetDir").toString());
        ui->lineEditTestSet->setText(setting.value("TestSetDir").toString());
    }
    setting.endGroup();
}

void SettingDialog::on_pushButtonTrainSetBrowse_clicked()
{
    QString TrainSetDir = QFileDialog::getExistingDirectory(
                this,tr("Directory of Train Set"),
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    ui->lineEditTrainSet->setText(TrainSetDir);
}

void SettingDialog::on_pushButtonTestSetBrowse_clicked()
{
    QString TrainSetDir = QFileDialog::getExistingDirectory(
                this,tr("Directory of Train Set"),
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    ui->lineEditTestSet->setText(TrainSetDir);
}

void SettingDialog::on_buttonBox_accepted()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Directory");
    {
        setting.setValue("TrainSetDir",ui->lineEditTrainSet->text());
        setting.setValue("TestSetDir",ui->lineEditTestSet->text());
    }
    setting.endGroup();
}
