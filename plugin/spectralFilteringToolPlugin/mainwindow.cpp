#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowContextHelpButtonHint| Qt::WindowCloseButtonHint);

    spFilter = 0;

    if(ui->actionColor_Mode->isChecked())
        colorMode = true;

    uiEnabler(false);
}

void MainWindow::resizeEvent(QResizeEvent * e)
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    sendSig();
}

MainWindow::~MainWindow()
{
    delete spFilter;
    delete ui;
}


void MainWindow::uiEnabler(bool c)
{
    //ui control
    ui->actionSave_image->setEnabled(c);
    ui->actionExport_to_Main->setEnabled(c);
}

void MainWindow::setFilter(cv::Mat &img)
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

void MainWindow::setShowSpectral(cv::Mat& img)
{
    if(img.empty())
        return;
    cv::Mat show;
    //img.convertTo(show, CV_8UC1, 255, 0);
    myCV::myCvtColor(img, show, myCV::GRAY2GBR);
    QImage&& qshow = QImage(show.data, show.cols, show.rows, show.step, QImage::Format_RGB888).rgbSwapped();
    qshow = qshow.scaled(ui->label_showSpectral->width(), ui->label_showSpectral->height(), Qt::KeepAspectRatio);
    ui->label_showSpectral->setPixmap(QPixmap::fromImage(qshow));
}
void MainWindow::setShowResult(cv::Mat& img)
{
    if(img.empty())
        return;
    cv::Mat show;
    if(img.type()==CV_8UC1)
    {
        myCV::myCvtColor(img, show, myCV::GRAY2GBR);
    }
    else
    {
        show = img.clone();
    }
    QImage qshow = QImage(show.data, show.cols, show.rows, show.step, QImage::Format_RGB888).rgbSwapped();
    qshow = qshow.scaled(ui->label_showSpectral->width(), ui->label_showSpectral->height(), Qt::KeepAspectRatio);
    ui->label_showResult->setPixmap(QPixmap::fromImage(qshow));
}

void MainWindow::initialSpectral()
{
    if(image.empty())
        return;
    clock_t tempT1 = clock();
    if(spFilter == 0)
    {
        spFilter = new myCV::spectralFiltering(image, ui->actionColor_Mode->isChecked(), ui->radioButton_homomorphic->isChecked());
    }
    else
    {
        spFilter->feedImage(image, ui->actionColor_Mode->isChecked(), ui->radioButton_homomorphic->isChecked());
    }
    int barMaximun = spFilter->getSpectralReal().rows > spFilter->getSpectralReal().cols ?
                     spFilter->getSpectralReal().rows / 2 : spFilter->getSpectralReal().cols / 2;
    ui->horizontalSlider_filterThreshold->setMaximum(barMaximun);
    ui->horizontalSlider_filterThreshold->setMinimum(0);
    ui->horizontalSlider_filterThreshold->setValue(ui->horizontalSlider_filterThreshold->maximum());
    ui->label_threshold->setText(QString::number(ui->horizontalSlider_filterThreshold->maximum()));

    tempT1 = clock() - tempT1;
    clock_t tempT2 = clock();
    spFilter->getResult(image);
    tempT2 = clock() - tempT2;

    setFilter(cv::Mat());
    setShowSpectral(spFilter->getSpectralIntensity());
    setShowResult(image);
    ui->statusbar->showMessage("Spectral filtering done. FFT time: "+QString::number((float)tempT1/CLOCKS_PER_SEC)+" sec."+
                                                         " iFFT time: "+QString::number((float)tempT2/CLOCKS_PER_SEC)+" sec.");
    uiEnabler(true);
}

void MainWindow::drawHomomorphic()
{
    ui->filter2DPlot->clearGraphs();
    cv::Mat data = spFilter->getFilter();
    int length = data.cols > data.rows ? data.cols / 2 : data.rows / 2;
    QVector<double> x(length);
    QVector<double> y(length);

    if(data.cols > data.rows)
    {
        #pragma omp parallel for
        for(int i = 0; i < x.size(); i++)
        {
            y[i] = data.at<float>(data.rows / 2, i + x.size());
            x[i] = i;
        }
    }
    else
    {
        #pragma omp parallel for
        for(int j = 0; j < x.size(); j++)
        {
            y[j] = data.at<float>(j + x.size(), data.cols / 2);
            x[j] = j;
        }
    }
    ui->filter2DPlot->addGraph();
    ui->filter2DPlot->graph(0)->setData(x, y);
    ui->filter2DPlot->xAxis->setRange(0, x.size());
    ui->filter2DPlot->yAxis->setRange(0, ui->doubleSpinBox_gammaHigh->maximum());
    ui->filter2DPlot->replot();
}

void MainWindow::on_actionOpen_image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");

    if(fileName.isEmpty())
        return;
    image = cv::imread(fileName.toStdString());
    if(image.empty())
        return;

    isImageImported = false;
    initialSpectral();
}

void MainWindow::on_actionSave_image_triggered()
{
    if(image.empty())
        return;

    QString fileName = QFileDialog::getSaveFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");

    if(fileName.isEmpty())
        return;

    cv::imwrite(fileName.toStdString(),image);
}

void MainWindow::on_actionImport_from_Main_triggered()
{
    sendImageRequest();
}

void MainWindow::receiveRequestedImage()
{
    image = _from_Main_img.clone();

    if(image.empty())
    {
        QMessageBox::critical(this,"Error","No image is opened in main program.");
        return;
    }

    isImageImported = true;
    initialSpectral();
}

void MainWindow::on_actionExport_to_Main_triggered()
{
    if(image.empty())
        return;

    if(isImageImported)
        sendExportImg(image, true, false);
    else
        sendExportImg(image, true, true);
}

void MainWindow::on_pushButton_applyFilter_clicked()
{
    if(image.empty())
        return;

    spFilter->computeFFT();
    clock_t tempT2 = clock();
    spFilter->getResult(image);
    tempT2 = clock() - tempT2;

    setShowSpectral(spFilter->getSpectralIntensity());
    setShowResult(image);
    ui->statusbar->showMessage("Spectral filtering done. iFFT time: "+QString::number((float)tempT2/CLOCKS_PER_SEC)+" sec.");
}

void MainWindow::on_pushButton__resetFilter_clicked()
{
    if(image.empty())
        return;
    spFilter->noFilter();
    setFilter(spFilter->getFilter());
    on_pushButton_applyFilter_clicked();
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_horizontalSlider_filterThreshold_sliderMoved(int position)
{
    ui->label_threshold->setText(QString::number(position));
}

void MainWindow::on_horizontalSlider_filterThreshold_sliderReleased()
{
    if(image.empty())
        return;
    if(ui->radioButton_homomorphic->isChecked())
    {
        spFilter->setHomomorphic(ui->doubleSpinBox_gammaHigh->value(), ui->doubleSpinBox_gammaLow->value(), 1.0);
    }
    else
    {
        spFilter->setHomomorphic(1.0, 0.0, 1.0);
    }

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
            spFilter->genLowPassFilter(myCV::FilterAlgorithm::gaussian, ui->horizontalSlider_filterThreshold->value());
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
            spFilter->genHighPassFilter(myCV::FilterAlgorithm::gaussian, ui->horizontalSlider_filterThreshold->value());
        }
    }
    else if(ui->radioButton_homomorphic->isChecked())
    {
        if(ui->radioButton_gaussianAlgorithm->isChecked())
        {
            spFilter->genHighPassFilter(myCV::FilterAlgorithm::gaussian, ui->horizontalSlider_filterThreshold->value());
        }
    }
    drawHomomorphic();
    setFilter(spFilter->getFilter());
}

void MainWindow::on_radioButton_idealAlgorithm_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_radioButton_butterworthAlgorithm_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_radioButton_gaussianAlgorithm_clicked()
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_radioButton_lowPassType_clicked()
{
    setHomoUI();
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_radioButton_highPassType_clicked()
{
    setHomoUI();
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_radioButton_homomorphic_clicked()
{
    setHomoUI();
}

void MainWindow::setHomoUI()
{
    ui->label_gammaHigh->setEnabled(ui->radioButton_homomorphic->isChecked());
    ui->label_gammaLow->setEnabled(ui->radioButton_homomorphic->isChecked());
    ui->label_c->setEnabled(ui->radioButton_homomorphic->isChecked());
    ui->doubleSpinBox_gammaHigh->setEnabled(ui->radioButton_homomorphic->isChecked());
    ui->doubleSpinBox_gammaLow->setEnabled(ui->radioButton_homomorphic->isChecked());
    ui->doubleSpinBox_c->setEnabled(ui->radioButton_homomorphic->isChecked());

    if(ui->radioButton_homomorphic->isChecked())
    {
        ui->radioButton_gaussianAlgorithm->setChecked(ui->radioButton_homomorphic->isChecked());
    }
    ui->radioButton_butterworthAlgorithm->setEnabled(!ui->radioButton_homomorphic->isChecked());
    ui->radioButton_idealAlgorithm->setEnabled(!ui->radioButton_homomorphic->isChecked());

    if(image.empty())
        return;

    clock_t tempT1 = clock();
    spFilter->changeGHPFMode(ui->radioButton_homomorphic->isChecked());
    tempT1 = clock() - tempT1;

    clock_t tempT2 = clock();
    spFilter->getResult(image);
    tempT2 = clock() - tempT2;

    setShowSpectral(spFilter->getSpectralIntensity());
    setShowResult(image);
    ui->statusbar->showMessage("Spectral filtering done. FFT time: "+QString::number((float)tempT1/CLOCKS_PER_SEC)+" sec."+
                                                         " iFFT time: "+QString::number((float)tempT2/CLOCKS_PER_SEC)+" sec.");

}

void MainWindow::on_spinBox_butterworthN_valueChanged(int arg1)
{
    if(ui->radioButton_butterworthAlgorithm->isChecked())
    {
        on_horizontalSlider_filterThreshold_sliderReleased();
    }
}

void MainWindow::on_doubleSpinBox_gammaHigh_valueChanged(double arg1)
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_doubleSpinBox_gammaLow_valueChanged(double arg1)
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_doubleSpinBox_c_valueChanged(double arg1)
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void MainWindow::on_actionColor_Mode_triggered()
{
     if(colorMode == ui->actionColor_Mode->isChecked())
         return;

     colorMode = ui->actionColor_Mode->isChecked();
     if(image.empty())
         return;

     clock_t tempT1 = clock();
     spFilter->changeColorMode(colorMode);
     tempT1 = clock() - tempT1;

     clock_t tempT2 = clock();
     spFilter->getResult(image);
     tempT2 = clock() - tempT2;

     setShowSpectral(spFilter->getSpectralIntensity());
     setShowResult(image);
     ui->statusbar->showMessage("Spectral filtering done. FFT time: "+QString::number((float)tempT1/CLOCKS_PER_SEC)+" sec."+
                                                          " iFFT time: "+QString::number((float)tempT2/CLOCKS_PER_SEC)+" sec.");
}

//Will delete soon as the code is checked.
/*
void MainWindow::on_pushButton_clicked()
{
    cv::Mat R, I;

    cv::Mat tmp;
    if(image.type()==CV_8UC3)
        myCV::myCvtColor(image, tmp, myCV::BGR2GRAY);
    else
        tmp = image.clone();
    cv::Mat dst(image.rows, image.cols, CV_32FC1);
    int j, i;
    #pragma omp parallel for private(i)
    for(j = 0; j < tmp.rows; j++)
    {
        for(i = 0; i < tmp.cols; i++)
        {
            dst.at<float>(j,i) = log(1.0 + (float)tmp.at<uchar>(j, i));
        }
    }

    myCV::FFT2D(dst, R, I);
    cv::imshow("123213213", R);
    cv::Mat filter = spFilter->getFilter();
    if(!filter.empty())
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < R.rows; j++)
        {
            for(i = 0; i < I.cols; i++)
            {
                R.at<float>(j,i) *= filter.at<float>(j,i);
                I.at<float>(j,i) *= filter.at<float>(j,i);
            }
        }
    }
    cv::Mat dst2(image.rows, image.cols, CV_8UC1);
    myCV::iFFT2DHomo(R, I, dst2, tmp.cols, tmp.rows);


     cv::imshow("final", dst2);

}*/
