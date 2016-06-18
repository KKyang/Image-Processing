#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>
#include <time.h>

#include "qlabelbar.h"
#include "opencv2/opencv.hpp"
#include "imgcore.h"
#include "imageprocessing.h"

#include "split.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setImageFromMain(cv::Mat &img){_from_Main_img = img.clone();}
signals:
    void sendSig();
private slots:
    void resizeEvent(QResizeEvent * e);
    void closeEvent(QCloseEvent *event);
    void on_label_pseudoBar_sendHoriDis(int dx);
    void on_label_pseudoBar_released();

    void on_actionOpen_image_triggered();

    void on_radioButton_hsv_clicked();

    void on_radioButton_rgb_clicked();

    void on_radioButton_cmy_clicked();

    void on_radioButton_xyz_clicked();

    void on_radioButton_cielab_clicked();

    void on_radioButton_yuv_clicked();

    void on_radioButton_gray_clicked();

    void on_radioButton_pseudoColor_clicked();

    void on_actionImport_from_Main_triggered();

private:
    void setShowImage(cv::Mat &img);
    void setShowImage(std::vector<cv::Mat> &imgs);
    void guiOnOff();
    Ui::MainWindow *ui;

    //cv::Mat originImg;
    cv::Mat _from_Main_img;
    cv::Mat image;
    int colorType = myCV::RGB;
    bool isInitial=false;
    int pic_status = 0;  //1 - gray, 2- pseudo, 3- else
    int pseudo_pos = 0;

    clock_t spendT;
};

#endif // MAINWINDOW_H
