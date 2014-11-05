#include "dialogfuzzylogic.h"
#include "ui_dialogfuzzylogic.h"

DialogFuzzyLogic::DialogFuzzyLogic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFuzzyLogic)
{
    ui->setupUi(this);

    bell.resize(511);
    triangWH.resize(256);
    triangBL.resize(256);
    Bell.mean     = 0;
    Bell.b        = 230;

    TriangWH.center = 255;
    TriangWH.left   = 205;
    TriangWH.right  = 1;
    TriangBL.center = 0;
    TriangBL.left   = -1;
    TriangBL.right  = 200;

    ui->spinBox_bellThreshold->setValue(Bell.b);
    ui->horizontalSlider_bellThreshold->setValue(Bell.b);
    ui->spinBox_outputLeftBound->setValue(TriangWH.left);
    ui->horizontalSlider_outputLeftBound->setValue(TriangWH.left);
    ui->spinBox_outputLeftBound->setValue(TriangBL.right);
    ui->horizontalSlider_outputLeftBound->setValue(TriangBL.right);
}

DialogFuzzyLogic::~DialogFuzzyLogic()
{
    delete ui;
}

void DialogFuzzyLogic::drawBell()
{
    ui->bellShapePlot->clearGraphs();
    QVector<double> x(511);
    int i=0;
    #pragma omp parallel for private(i)
    for(i = 0; i < 511 ; i++)
    {
        bell[i] = bellCurve(i-255, Bell.b, Bell.mean);
        x[i] = i-255;
    }
    ui->bellShapePlot->addGraph();
    ui->bellShapePlot->graph(0)->setData(x, bell);
    ui->bellShapePlot->xAxis->setRange(-255,255);
    ui->bellShapePlot->yAxis->setRange(0,1);
    ui->bellShapePlot->replot();
}

void DialogFuzzyLogic::drawTriang()
{
    ui->triangPlot->clearGraphs();
    QVector<double> x2(256);
    int i=0;
    #pragma omp parallel for private(i)
    for(i=0;i < 256; i++)
    {
        triangWH[i] = triangShape(i, TriangWH.center, TriangWH.left, TriangWH.right);
        x2[i] = i;
    }
    #pragma omp parallel for private(i)
    for(i=0;i < 256; i++)
    {
        triangBL[i] = triangShape(i, TriangBL.center, TriangBL.left, TriangBL.right);
    }
    ui->triangPlot->addGraph();
    ui->triangPlot->graph(0)->setData(x2,triangWH);
    ui->triangPlot->addGraph();
    ui->triangPlot->graph(1)->setData(x2,triangBL);
    ui->triangPlot->xAxis->setRange(0,255);
    ui->triangPlot->yAxis->setRange(0,1);
    ui->triangPlot->replot();
}

void DialogFuzzyLogic::setShowImage(cv::Mat &img)
{
    cv::Mat show;
    myCV::myCvtColor(img, show, myCV::GRAY2GBR);
    QImage qshow = QImage(show.data, show.rows, show.cols, show.step, QImage::Format_RGB888).rgbSwapped();
    if(qshow.width() > ui->label_fuzzy->width() )
    {
        qshow = qshow.scaled(ui->label_fuzzy->width(), ui->label_fuzzy->height(), Qt::KeepAspectRatio);
    }
    if(qshow.height() > ui->label_fuzzy->height())
    {
        qshow = qshow.scaled(ui->label_fuzzy->width(), ui->label_fuzzy->height(), Qt::KeepAspectRatio);
    }
    ui->label_fuzzy->setPixmap(QPixmap::fromImage(qshow));
}

void DialogFuzzyLogic::readImage(cv::Mat &img)
{
    originImg = img.clone();

    cv::Mat result;
    fuzzy.setFuzzyFunctionType(1);
    fuzzy.setBellCurveProperties(Bell.mean, Bell.b);
    fuzzy.setTriangProperties(TriangWH.center, TriangWH.left, TriangWH.right);
    fuzzy.getBoundaries(originImg, result);
    setShowImage(result);
}

void DialogFuzzyLogic::computeResult(cv::Mat &outputResult)
{
    fuzzy.setFuzzyFunctionType(1);
    fuzzy.setBellCurveProperties(Bell.mean, Bell.b);
    fuzzy.setTriangProperties(TriangWH.center, TriangWH.left, TriangWH.right);
    fuzzy.getBoundaries(originImg, outputResult);

}

double DialogFuzzyLogic::sShape(int x, int a, int b, int c)
{
    if(x < a)
    {
        return 0;
    }
    else if(x >= a && x <= b)
    {
        return 2*pow((double)(x-a)/(double)(c-a),2);
    }
    else if(x > b && x <= c)
    {
        return 1-2*pow((double)(x-c)/(double)(c-a),2);
    }
    else
        return 1;
}

double DialogFuzzyLogic::bellCurve(int x, int b, int c)
{
    if(x <= c)
    {
        return sShape(x, c-b, c-((double)b/2),c);
    }
    else
    {
        return 1 - sShape(x, c, c+((double)b/2), c+b);
    }
}

double DialogFuzzyLogic::triangShape(int x, int center, int a, int b)
{
    if(center - a <= x && x < center)
    {
        return  1-((center-x)/(double)a);
    }
    else if(center <= x && x < center + b)
    {
        return 1-((x-center)/(double)b);
    }
    else
    {
        return 0;
    }
}

void DialogFuzzyLogic::on_horizontalSlider_bellThreshold_valueChanged(int value)
{
    ui->spinBox_bellThreshold->setValue(value);
}

void DialogFuzzyLogic::on_spinBox_bellThreshold_valueChanged(int arg1)
{
    ui->horizontalSlider_bellThreshold->setValue(arg1);
    Bell.b = ui->spinBox_bellThreshold->value();
    drawBell();
}

void DialogFuzzyLogic::on_horizontalSlider_outputLeftBound_valueChanged(int value)
{
    ui->spinBox_outputLeftBound->setValue(value);
}

void DialogFuzzyLogic::on_spinBox_outputLeftBound_valueChanged(int arg1)
{
    ui->horizontalSlider_outputLeftBound->setValue(arg1);
    TriangWH.left = ui->spinBox_outputLeftBound->value();
    drawTriang();
}

void DialogFuzzyLogic::on_horizontalSlider_outputRightBound_valueChanged(int value)
{
    ui->spinBox_outputRightBound->setValue(value);
}

void DialogFuzzyLogic::on_spinBox_outputRightBound_valueChanged(int arg1)
{
    ui->horizontalSlider_outputRightBound->setValue(arg1);
    TriangBL.right = ui->spinBox_outputRightBound->value();
    drawTriang();
}

void DialogFuzzyLogic::on_horizontalSlider_bellThreshold_sliderReleased()
{
    cv::Mat result;
    computeResult(result);
    setShowImage(result);
}

void DialogFuzzyLogic::on_spinBox_bellThreshold_editingFinished()
{
    cv::Mat result;
    computeResult(result);
    setShowImage(result);
}

void DialogFuzzyLogic::on_horizontalSlider_outputLeftBound_sliderReleased()
{
    cv::Mat result;
    computeResult(result);
    setShowImage(result);
}

void DialogFuzzyLogic::on_spinBox_outputLeftBound_editingFinished()
{
    cv::Mat result;
    computeResult(result);
    setShowImage(result);
}

void DialogFuzzyLogic::on_horizontalSlider_outputRightBound_sliderReleased()
{
    cv::Mat result;
    computeResult(result);
    setShowImage(result);
}

void DialogFuzzyLogic::on_spinBox_outputRightBound_editingFinished()
{
    cv::Mat result;
    computeResult(result);
    setShowImage(result);
}
