#include "traintab.h"
#include "ui_traintab.h"
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QList>
#include "SignRecognitionToolkit.h"

TrainTab::TrainTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainTab)
{
    ui->setupUi(this);
    QAction *a= new QAction(QIcon(":/image/resource/next.png"),"hehe",ui->listWidget);
    ui->listWidget->addAction(a);
}

TrainTab::~TrainTab()
{
    delete ui;
}

void TrainTab::SetImageItems(const QStringList &list)
{
    QStringList errorList;
    foreach (QString imagePath, list) {
        if (QFile(imagePath).exists())
        {
            QListWidgetItem *item = new QListWidgetItem(QIcon(imagePath),imagePath);
            ui->listWidget->addItem(item);
        }
        else
            errorList.push_back(imagePath);
    }
    if (!errorList.empty())
    {
        QMessageBox::warning(this,tr("Some files lost"),tr("The lost files are as follow:\n")+errorList.join("\n"));
    }

}

void TrainTab::SetConfigPath(const QString &path)
{
    ui->lineEditConfigPath->setText(path);
}

QStringList TrainTab::GetImageItems()
{
    QStringList stringList;
    for (int i=0;i<ui->listWidget->count();++i)
    {
        stringList.push_back(ui->listWidget->item(i)->text());
    }
    return stringList;
}

QString TrainTab::GetConfigPath()
{
    return ui->lineEditConfigPath->text();
}

void TrainTab::on_pushButton_clicked()
{
    QString imageDirString;
    QSettings setting("config.ini",QSettings::IniFormat);
    setting.beginGroup("Directory");
    imageDirString = setting.value("TrainSetDir").toString();
    setting.endGroup();

    QStringList fileList = QFileDialog::getOpenFileNames(this,tr("Add image files"),imageDirString,tr("Image Files(*.bmp *.jpg *.ppm *.png)"));
    std::sort(fileList.begin(),fileList.end());
    SetImageItems(fileList);
}


void TrainTab::on_listWidget_itemSelectionChanged()
{
    QList<QListWidgetItem*> widgetList = ui->listWidget->selectedItems();
    if (widgetList.empty())
    {
        ui->label->clear();
        return;
    }
    else
    {
        ui->label->setPixmap(QPixmap(widgetList[widgetList.size()-1]->text()));
        ui->label->show();
    }
}

void TrainTab::on_pushButtonSelectAll_clicked()
{
    ui->listWidget->selectAll();
}

void TrainTab::on_pushButtonSelectNone_clicked()
{
    ui->listWidget->clearSelection();
}

void TrainTab::on_pushButtonRemoveFile_clicked()
{
    for (int i=0;i<ui->listWidget->count();++i)
    {
        QListWidgetItem *item = ui->listWidget->item(i);
        if (item->isSelected())
        {
            ui->listWidget->takeItem(i);
            delete item;
            --i;
        }
    }
    on_listWidget_itemSelectionChanged();
}

void TrainTab::on_pushButtonConfigBrowse_clicked()
{
    QString configFile = QFileDialog::getOpenFileName(this,tr("Open config file of this class"),ui->lineEditConfigPath->text());
    ui->lineEditConfigPath->setText(configFile);
}

void TrainTab::on_pushButton_2_clicked()
{


}
