#ifndef TRAINDIALOG_H
#define TRAINDIALOG_H

#include <QDialog>
class TrainTab;

namespace Ui {
class TrainDialog;
}

class QFileInfo;
class ClassInfo
{
public:
    QString className;
    QList<QFileInfo> fileList;
};

class TrainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrainDialog(bool isTraining,QWidget *parent = 0);
    ~TrainDialog();

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonRemove_clicked();
    void on_pushButtonRemoveAll_clicked();
    void on_buttonBox_accepted();

private:
    Ui::TrainDialog *ui;
    QList<TrainTab*> trainTab;
    bool isTrain;
    void InitWidget();
    void SaveTrainDataInfo();
    void RenameWindowTitle();

};

#endif // TRAINDIALOG_H
