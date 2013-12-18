#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

class QFileInfo;
class QLineEdit;
class QStandardItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:   
    void on_actionSettings_triggered();
    void on_actionNextImage_triggered();
    void on_actionPreviousImage_triggered();
    void on_animate();
    void on_actionAnimation_toggled(bool arg1);
    void on_actionFirstImage_triggered();
    void on_actionLastImage_triggered();
    void on_actionTestCanny_triggered();
    void on_actionSetTrainData_triggered();
    void on_actionSetTestData_triggered();
    void on_actionBrowseTestSet_triggered();
    void on_actionSVMTraining_triggered();
    void on_actionOpenSingleImage_triggered();
    void on_actionSVMTestingForSingleImage_triggered();
    void on_actionMLPTestingForSingleImage_triggered();
    void on_actionMLPTraining_triggered();


    void on_actionMLPTestingForTestData_triggered();

    void on_actionLanguage_triggered();

private:
    Ui::MainWindow *ui;
    QLineEdit* lineEditCurrentID;
    QGraphicsScene *graphicsScene;
    QList<QFileInfo> imageFileList;

    QTimer *timer;
    int imageListID;
    QString singleImagePath;

    void ShowCurrentImage();
    void resizeEvent(QResizeEvent *event);
    void SetEnableNavigateActions(bool enabled);
    void SetSingleImageActions(bool enabled);
};

#endif // MAINWINDOW_H
