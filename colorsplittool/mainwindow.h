#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <time.h>

#include "localsocketipc.h"
#include "sharememory.h"

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

private slots:
    void closeEvent(QCloseEvent *event);
    void socketClientStatus(int status);
    void socketIcpMessage(QString message);

    void on_actionOpen_image_triggered();

    void on_radioButton_hsv_clicked();

    void on_radioButton_rgb_clicked();

    void on_radioButton_cmy_clicked();

    void on_radioButton_xyz_clicked();

    void on_radioButton_cielab_clicked();

    void on_radioButton_yuv_clicked();

    void on_radioButton_gray_clicked();

    void on_checkBox_pseudoSwitch_clicked();

    void on_horizontalSlider_pseudoValue_sliderReleased();

private:
    void setShowImage(cv::Mat &img);
    void setShowImage(std::vector<cv::Mat> &imgs);
    void guiOnOff();
    Ui::MainWindow *ui;

    //cv::Mat originImg;
    cv::Mat image;
    int colorType = myCV::RGB;
    bool isInitial=false;
    bool isGray = false;

    clock_t spendT;

    LocalSocketIpcClient* m_client;
    LocalSocketIpcServer* m_server;
    shareMemory* mem;
};

#endif // MAINWINDOW_H
