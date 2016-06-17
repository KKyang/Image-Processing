#ifndef SUBWINDOWWIDGET_H
#define SUBWINDOWWIDGET_H

#pragma once
#include <QWidget>
#include <QMdiSubWindow>
#include "plugininterfaces.h"
#ifdef HAVE_OPENCV
#include "opencv2/opencv.hpp"
#endif

#include "imageprocessing/fft.h"
#include "imageprocessing/fuzzylogic.h"

#include "imgcore.h"
#include "imageprocessing.h"

namespace Ui {
class subWindowWidget;
}

class subWindowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit subWindowWidget(QWidget *parent = 0);
    ~subWindowWidget();
    void setImage(cv::Mat &img);
    void setImage(QImage &img);

private:
    Ui::subWindowWidget *ui;
};

namespace Ui {
    class subWindow;
}

class subWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    explicit subWindow(QWidget *parent = 0);
    ~subWindow();
    void backupImage();
    void refreshImg();

    cv::Mat _img;
public slots:
    void setImage(cv::Mat &img);
private:
    subWindowWidget *windowWidget;
    std::vector<cv::Mat> recoverImg;
    std::vector<cv::Mat> forwardImg;
    int recoverLimit = 5;

};

#endif // SUBWINDOWWIDGET_H
