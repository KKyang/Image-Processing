#include "spectralfiltertool.h"
#include "ui_spectralfiltertool.h"

spectralFilterTool::spectralFilterTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::spectralFilterTool)
{
    ui->setupUi(this);
    spFilter = 0;
}

spectralFilterTool::~spectralFilterTool()
{
    delete ui;
}

void spectralFilterTool::closeEvent(QCloseEvent *event)
{
    emit windowClosed(1);
}

void spectralFilterTool::setShowSpectral(cv::Mat& img)
{
    cv::Mat show;
    //Convert 32FC1 to 8UC1 and display on screen.
    img.convertTo(show, CV_8UC1, 255, 0);
    myCV::myCvtColor(show, show, myCV::GRAY2GBR);
    QImage qshow = QImage(show.data, show.cols, show.rows, show.step, QImage::Format_RGB888).rgbSwapped();
    qshow = qshow.scaled(ui->label_showSpectral->width(), ui->label_showSpectral->height(), Qt::KeepAspectRatio);
    ui->label_showSpectral->setPixmap(QPixmap::fromImage(qshow));
}

void spectralFilterTool::readImage(cv::Mat &img)
{
    originImg = img.clone();
}

void spectralFilterTool::computeSpectral()
{
    if(originImg.empty())
        return;

    spFilter = new myCV::spectralFiltering(originImg);
    setShowSpectral(spFilter->getSpectralReal());
    cv::Mat tmp;
    //spFilter->getResult(tmp);
    //cv::imshow("ifft", tmp);
}
