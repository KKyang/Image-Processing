#ifndef SPECTRALFILTERTOOL_H
#define SPECTRALFILTERTOOL_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <time.h>

#include "localsocketipc.h"

#include "opencv2/opencv.hpp"
#include "imgcore.h"
#include "imageprocessing.h"
#include "imageprocessing/spectralfiltering.h"

#include "smartlabel.h"

namespace Ui {
class spectralFilterTool;
}

class spectralFilterTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit spectralFilterTool(QWidget *parent = 0);
    ~spectralFilterTool();
    void messageBroadCast(QString title, QString msg);
    void readImage(cv::Mat &img);
    void initialSpectral();

    void getResult(cv::Mat &img);

private slots:
    void closeEvent(QCloseEvent *event);
    void socketIcpMessage(QString message);
    void socketIcpQImage(QImage img);

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

    void on_actionExport_image_to_Simple_Processing_triggered();

signals:
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

    LocalSocketIpcClient* m_client;
    LocalSocketIpcServer* m_server;
};

#endif // SPECTRALFILTERTOOL_H
