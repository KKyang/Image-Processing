#ifndef SUBWINDOWWIDGET_H
#define SUBWINDOWWIDGET_H

#pragma once
#include <QWidget>
#include <QMdiSubWindow>
#include "plugininterfaces.h"
#ifdef HAVE_OPENCV
#include "opencv2/opencv.hpp"
#endif
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

    cv::Mat _img;
public slots:
    void setImage(cv::Mat &img);
private:
    subWindowWidget *windowWidget;

};

#endif // SUBWINDOWWIDGET_H
