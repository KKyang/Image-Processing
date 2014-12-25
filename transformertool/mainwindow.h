#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <time.h>

#include "localsocketipc.h"
#include "sharememory.h"
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

private slots:
    void closeEvent(QCloseEvent *event);
    void socketClientStatus(int status);
    void socketIcpMessage(QString message);

    void on_actionOpen_image_triggered();

    void on_actionImport_photo_from_Simple_Processing_triggered();

    void on_pushButton_tilt_clicked();

    void on_pushButton_wave_clicked();

    void on_pushButton_original_clicked();

    void on_pushButton_clicked();

    void on_pushButton_houghLine_clicked();

    void on_horizontalSlider_lineThreshold_sliderMoved(int position);

    void on_spinBox_lineThresholdValue_editingFinished();

    void on_actionExport_Image_to_Simple_Processing_triggered();

    void on_pushButton_houghRectangle_clicked();

private:
    void setShowImage(cv::Mat &img);
    void setShowImage(std::vector<cv::Mat> &imgs);
    Ui::MainWindow *ui;

    //cv::Mat originImg;
    cv::Mat image;

    LocalSocketIpcClient* m_client;
    LocalSocketIpcServer* m_server;
    shareMemory* mem;
};

#endif // MAINWINDOW_H
