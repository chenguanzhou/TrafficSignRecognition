#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

private slots:
    void ReadParams();
    void WriteParams();
    void on_pushButtonTrainSetBrowse_clicked();
    void on_pushButtonTestSetBrowse_clicked();
    void on_buttonBox_accepted();

    void on_pushButtonTestSetBrowseSVM_clicked();

    void on_pushButtonTestSetBrowseMLP_clicked();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
