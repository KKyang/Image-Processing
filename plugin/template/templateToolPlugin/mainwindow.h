#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>

#include "opencv2/opencv.hpp"
#include "imgcore.h"
#include "imageprocessing.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
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

private:
    Ui::MainWindow *ui;
    bool isImageImported = false;
    cv::Mat _from_Main_img;
    cv::Mat image;
    QString openedFileName;
};

#endif // MAINWINDOW_H
