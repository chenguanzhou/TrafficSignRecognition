#include "traindialog.h"
#include "ui_traindialog.h"
#include "traintab.h"
#include <QMessageBox>
#include <QListWidget>
#include <QSettings>

TrainDialog::TrainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrainDialog)
{
    ui->setupUi(this);
    InitWidget();
}

TrainDialog::~TrainDialog()
{
    delete ui;
}

void TrainDialog::on_pushButtonAdd_clicked()
{
    ui->tabWidget->addTab(new TrainTab(),tr("The ")+QString::number(ui->tabWidget->count()+1)+tr("th Class"));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
}

void TrainDialog::on_TrainDialog_accepted()
{

}

void TrainDialog::InitWidget()
{
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("TrainDataConfig");
    int ClassCount = setting.value("Count").toInt();
    if (ClassCount == 0)  return;
    for (int i=0;i<ClassCount;++i)
    {
        QStringList stringList=setting.value(QString("Class")+QString::number(i)+"_Files").toStringList();
        QString configPath = setting.value(QString("Class")+QString::number(i)+"_Config").toString();
        TrainTab *tab = new TrainTab();
        tab->SetImageItems(stringList);
        tab->SetConfigPath(configPath);
        ui->tabWidget->addTab(tab,tr("The ")+QString::number(ui->tabWidget->count()+1)+tr("th Class"));
    }
    setting.endGroup();
}

void TrainDialog::SaveTrainDataInfo()
{
    QList<TrainTab*> trainTab = ui->tabWidget->findChildren<TrainTab *>();

    int index = 0;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("TrainDataConfig");
    setting.setValue("Count",QVariant(trainTab.size()));
    foreach (TrainTab* tab, trainTab)
    {
        setting.setValue(QString("Class")+QString::number(index)+"_Files" ,QVariant(tab->GetImageItems()));
        setting.setValue(QString("Class")+QString::number(index)+"_Config",QVariant(tab->GetConfigPath()));
        index++;
    }

    setting.endGroup();
}

void TrainDialog::RenameWindowTitle()
{
    for (int i=0;i<ui->tabWidget->count();++i)
        ui->tabWidget->setTabText(i,tr("The ")+QString::number(i+1)+tr("th Class"));
}

void TrainDialog::on_pushButtonRemove_clicked()
{
    if (ui->tabWidget->count()==0)
        return;
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    RenameWindowTitle();
}

void TrainDialog::on_pushButtonRemoveAll_clicked()
{
    ui->tabWidget->clear();
}

void TrainDialog::on_buttonBox_accepted()
{
    SaveTrainDataInfo();
}
