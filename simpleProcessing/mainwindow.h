#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QSettings>
#include <QString>

#include <time.h>
#include <vector>

#include "dialog/dialogblur.h"
#include "dialog/dialogfuzzylogic.h"
#include "dialog/dialogpreference.h"
#include "dialog/dialogresize.h"
#include "dialog/dialogsize.h"

#include "tool/dialogcustomfilter.h"
#include "tool/dialogimagesubtracttool.h"

#include "lib/imageprocessing/fuzzylogic.h"
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
    void receiveMousePress();
    void receiveMousePressPreview();
    void receiveImageSubsClose();

    void on_actionOpen_Image_triggered();

    void on_actionGrayscale_average_triggered();

    void on_actionGrayscale_luminosity_triggered();

    void on_actionResize_triggered();

    void on_actionBack_triggered();

    void on_actionForward_triggered();

    void on_actionThreshold_triggered();

    void on_actionHistogram_Equalization_triggered();

    void on_horizontalSlider_contrast_sliderReleased();

    void on_horizontalSlider_contrast_valueChanged(int value);

    void on_actionContrast_triggered();

    void on_actionBlur_triggered();

    void on_actionPreference_triggered();

    void on_actionOpenImage_triggered();

    void on_actionOptions_triggered();

    void on_horizontalSlider_grayResolution_sliderReleased();

    void on_horizontalSlider_grayResolution_valueChanged(int value);

    void on_actionGray_Resolution_Scale_triggered();

    void on_actionMedian_Filter_triggered();

    void on_actionMean_Filter_triggered();

    void on_actionFuzzy_Logic_triggered();

    void on_actionLaplacian_Filter_triggered();

    void on_actionCustom_Filter_triggered();

    void on_actionImage_Subtractor_triggered();

private:
    Ui::MainWindow *ui;
    DialogPreference *pref;
    void loadSettings();
    void saveSettings();

    void setShowImage(cv::Mat &img);
    void setShowImagePreview(cv::Mat &img);
    void setShowHistogram(cv::Mat &img);
    void backupImage(cv::Mat &img);

    dialogimagesubtracttool* ist;
    QString m_sSettingsFile;
    QString fileName;
    cv::Mat image, imagePreview;

    std::vector<cv::Mat> recoverImg;
    std::vector<cv::Mat> forwardImg;
    int recoverLimit = 5;

    clock_t spendT;


};

#endif // MAINWINDOW_H
