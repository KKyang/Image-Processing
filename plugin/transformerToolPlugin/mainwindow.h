#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>

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

    void uiEnabler(bool c);
    void setImageFromMain(cv::Mat &img){_from_Main_img = img.clone();}
    void receiveRequestedImage();
    QString getOpenedFileName(){return openedFileName;}
signals:
    void sendSig();
    void sendImageRequest();
    void sendExportImg(cv::Mat &img, bool isChanged = true, bool isNew = false);
private slots:
    void resizeEvent(QResizeEvent * e);
    void closeEvent(QCloseEvent *event);

    void on_actionOpen_image_triggered();

    void on_pushButton_tilt_clicked();

    void on_pushButton_wave_clicked();

    void on_pushButton_original_clicked();

    void on_pushButton_clicked();

    void on_pushButton_houghLine_clicked();

    void on_horizontalSlider_lineThreshold_sliderMoved(int position);

    void on_spinBox_lineThresholdValue_editingFinished();

    void on_pushButton_houghRectangle_clicked();

    void on_actionImport_from_Main_triggered();

    void on_actionExport_to_Main_triggered();

private:
    void setShowImage(cv::Mat &img);
    void setShowImage(std::vector<cv::Mat> &imgs);

    Ui::MainWindow *ui;
    bool isImageImported = false;
    cv::Mat _from_Main_img;
    cv::Mat image;
    QString openedFileName;
};

#endif // MAINWINDOW_H
