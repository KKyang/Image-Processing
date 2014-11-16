#ifndef SPECTRALFILTERTOOL_H
#define SPECTRALFILTERTOOL_H

#include <QMainWindow>
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
    void computeSpectral();

private slots:
    void closeEvent(QCloseEvent *event);

signals:
    void windowClosed(int num);

private:
    void setShowSpectral(cv::Mat& img);
    Ui::spectralFilterTool *ui;
    myCV::spectralFiltering *spFilter;
    cv::Mat originImg;
};

#endif // SPECTRALFILTERTOOL_H
