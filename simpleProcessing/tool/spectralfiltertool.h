#ifndef SPECTRALFILTERTOOL_H
#define SPECTRALFILTERTOOL_H

#include <QMainWindow>
#include <QFileDialog>
#include <time.h>
#include "opencv2/opencv.hpp"
#include "../lib/imageprocessing/spectralfiltering.h"
#include "../lib/imageprocessing.h"
#include "../smartlabel.h"

namespace Ui {
class spectralFilterTool;
}

class spectralFilterTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit spectralFilterTool(QWidget *parent = 0);
    ~spectralFilterTool();
    void readImage(cv::Mat &img);
    void initialSpectral();

private slots:
    void closeEvent(QCloseEvent *event);

    void on_actionImport_image_from_SimpleProcessing_triggered();

    void on_pushButton_applyFilter_clicked();

    void on_pushButton__resetFilter_clicked();

    void on_horizontalSlider_filterThreshold_sliderMoved(int position);

    void on_horizontalSlider_filterThreshold_sliderReleased();

    void on_radioButton_idealAlgorithm_clicked();

    void on_radioButton_butterworthAlgorithm_clicked();

    void on_radioButton_gaussianAlgorithm_clicked();

    void on_radioButton_lowPassType_clicked();

    void on_radioButton_highPassType_clicked();

    void on_spinBox_butterworthN_valueChanged(int arg1);

    void on_actionOpen_image_triggered();

    void on_checkBox_useHomomorphic_clicked();

    void on_doubleSpinBox_gammaHigh_valueChanged(double arg1);

    void on_doubleSpinBox_gammaLow_valueChanged(double arg1);

signals:
    void getImgFromMain(int num);
    void windowClosed(int num);

private:
    void drawHomomorphic();
    void setFilter(cv::Mat &img);
    void setShowSpectral(cv::Mat &imgR, cv::Mat &imgI);
    void setShowResult(cv::Mat &img);
    Ui::spectralFilterTool *ui;
    myCV::spectralFiltering *spFilter;

    cv::Mat originImg;
    clock_t spendT;
};

#endif // SPECTRALFILTERTOOL_H
