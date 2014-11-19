#include "spectralfiltertool.h"
#include "ui_spectralfiltertool.h"

spectralFilterTool::spectralFilterTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::spectralFilterTool)
{
    ui->setupUi(this);
    spFilter = 0;

    if(ui->actionColor_Mode->isChecked())
        colorMode = true;

    m_client = new LocalSocketIpcClient("simpleProcessing", this);
    m_server = new LocalSocketIpcServer("spectralFilterTool", this);

    connect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
    mem = new shareMemory();
}

spectralFilterTool::~spectralFilterTool()
{
    delete spFilter;
    delete ui;
}

void spectralFilterTool::closeEvent(QCloseEvent *event)
{
    if(mem)
        delete mem;
    emit windowClosed(1);
}

void spectralFilterTool::messageBroadCast(QString title, QString msg)
{
    QMessageBox::warning(0, title, msg);
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
        spFilter = new myCV::spectralFiltering(originImg, ui->actionColor_Mode->isChecked());
    }
    else
    {
        spFilter->feedImage(originImg, ui->actionColor_Mode->isChecked());
    }
    int barMaximun = spFilter->getSpectralReal().rows > spFilter->getSpectralReal().cols ?
                     spFilter->getSpectralReal().rows / 2 : spFilter->getSpectralReal().cols / 2;
    ui->horizontalSlider_filterThreshold->setMaximum(barMaximun);
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

void spectralFilterTool::drawHomomorphic()
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

void spectralFilterTool::getResult(cv::Mat &img)
{
    if(originImg.empty())
        return;

    img.release();
    spFilter->getResult(img);
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
}

void spectralFilterTool::on_pushButton_applyFilter_clicked()
{
    if(originImg.empty())
        return;

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
    if(originImg.empty())
        return;
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
    if(originImg.empty())
        return;
    if(ui->checkBox_useHomomorphic->isChecked())
    {
        spFilter->setHomomorphic(ui->doubleSpinBox_gammaHigh->value(), ui->doubleSpinBox_gammaLow->value());
    }
    else
    {
        spFilter->setHomomorphic(1.0, 0.0);
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
    drawHomomorphic();
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

void spectralFilterTool::on_checkBox_useHomomorphic_clicked()
{
    ui->label_gammaHigh->setEnabled(ui->checkBox_useHomomorphic->isChecked());
    ui->label_gammaLow->setEnabled(ui->checkBox_useHomomorphic->isChecked());
    ui->doubleSpinBox_gammaHigh->setEnabled(ui->checkBox_useHomomorphic->isChecked());
    ui->doubleSpinBox_gammaLow->setEnabled(ui->checkBox_useHomomorphic->isChecked());
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_doubleSpinBox_gammaHigh_valueChanged(double arg1)
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_doubleSpinBox_gammaLow_valueChanged(double arg1)
{
    on_horizontalSlider_filterThreshold_sliderReleased();
}

void spectralFilterTool::on_actionImport_image_from_SimpleProcessing_triggered()
{
    m_client->sendMessageToServer("requestImage");
}

void spectralFilterTool::on_actionExport_image_to_Simple_Processing_triggered()
{
    if(originImg.empty())
    {
        QMessageBox::warning(0, "Error", "No image is opened!");
        return;
    }
    m_client->sendMessageToServer("ImportImage");
}

void spectralFilterTool::socketIcpMessage(QString message)
{
    if(message == "requestImage")
    {
        if(originImg.empty())
        {
            m_client->sendMessageToServer("NoImage.");
            return;
        }
        cv::Mat tmp;
        spFilter->getResult(tmp);
        if(mem->addToSharedMemory(tmp))
            m_client->sendMessageToServer("ok "+QString::number(tmp.type()));
        else
            m_client->sendMessageToServer("Porting fail.");
    }
    else if(message.contains("ok "))
    {
        int imageType = message.right(3).toInt();
        if(mem->readFromSharedMemory(originImg, imageType))
        {
            initialSpectral();
            m_client->sendMessageToServer("done");
        }
    }
    else if(message == "done")
    {
        mem->requestDetach();
    }
    else
    {
        QMessageBox::warning(0, "Error", message);
    }
}

void spectralFilterTool::on_actionSave_image_triggered()
{

}

void spectralFilterTool::on_actionColor_Mode_triggered()
{
     if(colorMode == ui->actionColor_Mode->isChecked())
         return;

     colorMode = ui->actionColor_Mode->isChecked();
     cv::Mat tmp;
     spFilter->changeColorMode(colorMode);
     spFilter->getResult(tmp);
     setShowResult(tmp);
}
