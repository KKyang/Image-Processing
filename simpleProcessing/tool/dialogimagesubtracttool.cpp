#include "dialogimagesubtracttool.h"
#include "ui_dialogimagesubtracttool.h"

dialogimagesubtracttool::dialogimagesubtracttool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dialogimagesubtracttool)
{
    ui->setupUi(this);
}

dialogimagesubtracttool::~dialogimagesubtracttool()
{
    delete ui;
}

void dialogimagesubtracttool::setShowImage(cv::Mat &img, QLabel *label)
{
    cv::Mat show;
    if(img.type()==CV_8UC1)
        myCV::myCvtColor(img, show, myCV::GRAY2GBR);
    else
        show = img.clone();

    QImage qshow = QImage(show.data, show.cols, show.rows, show.step, QImage::Format_RGB888).rgbSwapped();
    qshow = qshow.scaled(label->width(), label->height(), Qt::KeepAspectRatio);
    label->setPixmap(QPixmap::fromImage(qshow));
}

void dialogimagesubtracttool::setOriginImage(cv::Mat &img)
{
    origin = img.clone();
    setShowImage(origin, ui->label_origin);
}

void dialogimagesubtracttool::on_actionOpen_Image_triggered()
{
    fileName_compare = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");
    if(fileName_compare.isEmpty())
        return;
    compare = cv::imread(fileName_compare.toStdString());
    if(!compare.empty())
    {
        setShowImage(compare, ui->label_compare);
        ui->statusbar->showMessage("Image: " + fileName_compare + " opened!");
    }

    cv::Mat originGray;
    if(origin.type() == CV_8UC3)
    {
        myCV::myCvtColor(origin, originGray, myCV::BGR2GRAY);
    }
    else
        originGray = origin.clone();
    cv::Mat compareGray;
    if(compare.type() == CV_8UC3)
    {
        myCV::myCvtColor(compare, compareGray, myCV::BGR2GRAY);
    }
    else
        compareGray = compare.clone();

    result = originGray - compareGray;
    setShowImage(result, ui->label_result);

    double average=0, SD=0;
    for(int j=0; j < result.rows;j++)
    {
        for(int i=0;i < result.cols;i++)
        {
            average += result.at<uchar>(j,i);
        }
    }
    average = average / (result.rows * result.cols);
    for(int j=0; j < result.rows;j++)
    {
        for(int i=0;i < result.cols;i++)
        {
            SD += pow(((double)result.at<uchar>(j,i)-average),2);
        }
    }
    SD = pow(SD / (result.rows * result.cols),0.5);
    ui->lcdNumber_avg->display(average);
    ui->lcdnumber_SD->display(SD);
}

void dialogimagesubtracttool::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
}
