#ifndef DIALOGIMAGESUBTRACTTOOL_H
#define DIALOGIMAGESUBTRACTTOOL_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLabel>
#include "../lib/imgcore.h"
#include "../lib/imageprocessing.h"
#include "opencv2/opencv.hpp"

namespace Ui {
class dialogimagesubtracttool;
}

class dialogimagesubtracttool : public QMainWindow
{
    Q_OBJECT

public:
    explicit dialogimagesubtracttool(QWidget *parent = 0);
    ~dialogimagesubtracttool();
    void setOriginImage(cv::Mat &img);
signals:
    void windowClosed();
private slots:
    void on_actionOpen_Image_triggered();

private:
    void setShowImage(cv::Mat &img, QLabel *label);
    void closeEvent(QCloseEvent *event);
    Ui::dialogimagesubtracttool *ui;
    cv::Mat origin, compare, result;
    QString fileName_compare;
};

#endif // DIALOGIMAGESUBTRACTTOOL_H
