#ifndef MLPDIALOG_H
#define MLPDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QStandardItemModel>
#include <opencv2/opencv.hpp>

namespace Ui {
class MLPDialog;
}

class MLPDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MLPDialog(cv::PCA &p, QWidget *parent = 0);
    ~MLPDialog();

    cv::ANN_MLP_TrainParams GetMLPParam();

private slots:
    void on_comboBoxTrainingMethod_currentIndexChanged(int index);
    void on_buttonBox_accepted();


private:
    Ui::MLPDialog *ui;
    QStandardItemModel *standardItemModel;
    int ClassCount;
    cv::PCA &pca;
    bool use_pca;
    int pca_count;

    void InitUI();

};

#endif // MLPDIALOG_H
