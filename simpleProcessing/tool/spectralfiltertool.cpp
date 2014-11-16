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
    delete spFilter;
    delete ui;
}

void spectralFilterTool::closeEvent(QCloseEvent *event)
{
    emit windowClosed(1);
}

void spectralFilterTool::setFilter(cv::Mat &img)
{
    cv::Mat show;
    if(img.empty())
    {
        QImage qshow(ui->label_showFilter->width(), ui->label_showFilter->height(), QImage::Format_RGB888);
        qshow.fill(Qt::white);
        ui->label_showFilter->setPixmap(QPixmap::fromImage(qshow));
        return;
    }
    //Convert 32FC1 to 8UC1 and display on screen.
    img.convertTo(show, CV_8UC1, 255, 0);
    myCV::myCvtColor(show, show, myCV::GRAY2GBR);
    QImage&& qshow = QImage(show.data, show.cols, show.rows, show.step, QImage::Format_RGB888).rgbSwapped();
    qshow = qshow.scaled(ui->label_showFilter->width(), ui->label_showFilter->height(), Qt::KeepAspectRatio);
    ui->label_showFilter->setPixmap(QPixmap::fromImage(qshow));
}

void spectralFilterTool::setShowSpectral(cv::Mat& imgR, cv::Mat &imgI)
{
    cv::Mat show;
    //Convert 32FC1 to 8UC1 and display on screen.
    cv::add(imgR, imgI, show);
    show.convertTo(show, CV_8UC1, 255, 0);
    myCV::myCvtColor(show, show, myCV::GRAY2GBR);
    QImage&& qshow = QImage(show.data, show.cols, show.rows, show.step, QImage::Format_RGB888).rgbSwapped();
    qshow = qshow.scaled(ui->label_showSpectral->width(), ui->label_showSpectral->height(), Qt::KeepAspectRatio);
    ui->label_showSpectral->setPixmap(QPixmap::fromImage(qshow));
}
void spectralFilterTool::setShowResult(cv::Mat& img)
{
    cv::Mat show;
    myCV::myCvtColor(img, show, myCV::GRAY2GBR);
    QImage qshow = QImage(show.data, show.cols, show.rows, show.step, QImage::Format_RGB888).rgbSwapped();
    qshow = qshow.scaled(ui->label_showSpectral->width(), ui->label_showSpectral->height(), Qt::KeepAspectRatio);
    ui->label_showResult->setPixmap(QPixmap::fromImage(qshow));
}

void spectralFilterTool::readImage(cv::Mat &img)
{
    originImg = img.clone();
}

void spectralFilterTool::initialSpectral()
{
    if(originImg.empty())
        return;
    clock_t tempT1 = clock();
    if(spFilter == 0)
    {
        spFilter = new myCV::spectralFiltering(originImg);
    }
    else
    {
        spFilter->feedImage(originImg);
    }

    ui->horizontalSlider_filterThreshold->setMaximum((int)(spFilter->getSpectralReal().rows / 2));
    ui->horizontalSlider_filterThreshold->setMinimum(0);
    ui->horizontalSlider_filterThreshold->setValue(ui->horizontalSlider_filterThreshold->maximum());
    ui->label_threshold->setText(QString::number(ui->horizontalSlider_filterThreshold->maximum()));

    tempT1 = clock() - tempT1;
    cv::Mat tmp;
    clock_t tempT2 = clock();
    spFilter->getResult(tmp);
    tempT2 = clock() - tempT2;
    setFilter(cv::Mat());
    setShowSpectral(spFilter->getSpectralReal(), spFilter->getSpectralImag());
    setShowResult(tmp);
    ui->statusbar->showMessage("Spectral filtering done. FFT time: "+QString::number((float)tempT1/CLOCKS_PER_SEC)+" sec."+
                                                         " iFFT time: "+QString::number((float)tempT2/CLOCKS_PER_SEC)+" sec.");
}


void spectralFilterTool::on_actionOpen_image_triggered()
{
    QString fileName_compare = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");
    if(fileName_compare.isEmpty())
        return;
    originImg = cv::imread(fileName_compare.toStdString());
    if(originImg.empty())
        return;

    initialSpectral();
    ui->statusbar->showMessage("Image: " + fileName_compare + " opened!");
}

void spectralFilterTool::on_actionImport_image_from_SimpleProcessing_triggered()
{
    emit getImgFromMain(1);
}

void spectralFilterTool::on_pushButton_applyFilter_clicked()
{
    spFilter->computeFFT();
    clock_t tempT2 = clock();
    cv::Mat tmp;
    spFilter->getResult(tmp);
    tempT2 = clock() - tempT2;

    setShowSpectral(spFilter->getSpectralReal(), spFilter->getSpectralImag());
    setShowResult(tmp);
    ui->statusbar->showMessage("Spectral filtering done. iFFT time: "+QString::number((float)tempT2/CLOCKS_PER_SEC)+" sec.");
}

void spectralFilterTool::on_pushButton__resetFilter_clicked()
{
    spFilter->noFilter();
    setFilter(spFilter->getFilter());
    on_pushButton_applyFilter_clicked();
}

void spectralFilterTool::on_horizontalSlider_filterThreshold_sliderMoved(int position)
{
    ui->label_threshold->setText(QString::number(position));
}

void spectralFilterTool::on_horizontalSlider_filterThreshold_sliderReleased()
{
    if(ui->radioButton_lowPassType->isChecked())
    {
        if(ui->radioButton_idealAlgorithm->isChecked())
            spFilter->genLowPassFilter(myCV::FilterAlgorithm::ideal, ui->horizontalSlider_filterThreshold->value());
        else if(ui->radioButton_butterworthAlgorithm->isChecked())
        {
            spFilter->setButterworth(ui->spinBox_butterworthN->value());
            spFilter->genLowPassFilter(myCV::FilterAlgorithm::butterworth, ui->horizontalSlider_filterThreshold->value());
        }
        else if(ui->radioButton_gaussianAlgorithm->isChecked())
        {
            spFilter->genLowPassFilter(myCV::FilterAlgorithm::butterworth, ui->horizontalSlider_filterThreshold->value());
        }
    }
    else if(ui->radioButton_highPassType->isChecked())
    {
        if(ui->radioButton_idealAlgorithm->isChecked())
            spFilter->genHighPassFilter(myCV::FilterAlgorithm::ideal, ui->horizontalSlider_filterThreshold->value());
        else if(ui->radioButton_butterworthAlgorithm->isChecked())
        {
            spFilter->setButterworth(ui->spinBox_butterworthN->value());
            spFilter->genHighPassFilter(myCV::FilterAlgorithm::butterworth, ui->horizontalSlider_filterThreshold->value());
        }
        else if(ui->radioButton_gaussianAlgorithm->isChecked())
        {
            spFilter->genHighPassFilter(myCV::FilterAlgorithm::butterworth, ui->horizontalSlider_filterThreshold->value());
        }
    }
    setFilter(spFilter->getFilter());
}

void spectralFilterTool::on_radioButton_idealAlgorithm_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_radioButton_butterworthAlgorithm_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_radioButton_gaussianAlgorithm_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_radioButton_lowPassType_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_radioButton_highPassType_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_spinBox_butterworthN_valueChanged(int arg1)
{
    if(ui->radioButton_butterworthAlgorithm->isChecked())
    {
        on_horizontalSlider_filterThreshold_sliderReleased();
    }
}
