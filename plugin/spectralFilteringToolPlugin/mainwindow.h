#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>
#include <time.h>

#include "opencv2/opencv.hpp"
#include "imgcore.h"
#include "imageprocessing.h"

#include "imageprocessing/spectralfiltering.h"
#include "imageprocessing/fft.h"

#include "qsmartlabel.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initialSpectral();
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

    void on_actionSave_image_triggered();

    void on_actionImport_from_Main_triggered();

    void on_actionExport_to_Main_triggered();

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

    void on_doubleSpinBox_gammaHigh_valueChanged(double arg1);

    void on_doubleSpinBox_gammaLow_valueChanged(double arg1);

    void on_actionColor_Mode_triggered();

    void on_radioButton_homomorphic_clicked();

    void on_doubleSpinBox_c_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    bool isImageImported = false;
    cv::Mat _from_Main_img;
    cv::Mat image;
    QString openedFileName;

    void setHomoUI();
    void drawHomomorphic();
    void setFilter(cv::Mat &img);
    void setShowSpectral(cv::Mat &img);
    void setShowResult(cv::Mat &img);
    myCV::spectralFiltering *spFilter;

    clock_t spendT;

    bool colorMode = false;
};

#endif // MAINWINDOW_H
