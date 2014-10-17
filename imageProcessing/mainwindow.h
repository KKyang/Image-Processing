#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QFileDialog>
#include <QString>

#include "dialog/dialogresize.h"

#include "lib/imgcore.h"
#include "lib/imageprocessing.h"
#include "smartgraphicsview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_Image_triggered();

    void on_actionGrayscale_average_triggered();

    void on_actionGrayscale_luminosity_triggered();

    void on_actionResize_triggered();

    void on_actionBack_triggered();

    void on_actionForward_triggered();

    void on_actionThreshold_triggered();

private:
    Ui::MainWindow *ui;
    void setShowImage(cv::Mat &img);
    void backupImage(cv::Mat &img);

    QString fileName;
    cv::Mat image;

    std::vector<cv::Mat> recoverImg;
    std::vector<cv::Mat> forwardImg;
    int recoverLimit = 5;


};

#endif // MAINWINDOW_H
