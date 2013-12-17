#ifndef TRAINTAB_H
#define TRAINTAB_H

#include <QWidget>

namespace Ui {
class TrainTab;
}

class TrainTab : public QWidget
{
    Q_OBJECT

public:
    explicit TrainTab(bool isTraining, QWidget *parent = 0);
    ~TrainTab();

    void SetImageItems(const QStringList &list);
    void SetConfigPath(const QString &path);
    QStringList GetImageItems();
    QString GetConfigPath();

private slots:
    void on_pushButton_clicked();
    void on_listWidget_itemSelectionChanged();
    void on_pushButtonSelectAll_clicked();
    void on_pushButtonSelectNone_clicked();
    void on_pushButtonRemoveFile_clicked();
    void on_pushButtonConfigBrowse_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::TrainTab *ui;
    bool isTrain;
};

#endif // TRAINTAB_H
