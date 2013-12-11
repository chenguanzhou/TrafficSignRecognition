#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

class QFileInfo;
class QLineEdit;

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
    void on_actionBrowseTestSet_triggered();

    void on_actionSVMTraining_triggered();

private:
    Ui::MainWindow *ui;
    QLineEdit* lineEditCurrentID;
    QGraphicsScene *graphicsScene;
    QList<QFileInfo> imageFileList;
    QTimer *timer;
    int imageListID;
    void ShowCurrentImage();
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
