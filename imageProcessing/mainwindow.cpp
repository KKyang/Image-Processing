#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_actionOpen_Image_triggered()
{
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"));
    image = cv::imread(fileName.toStdString());
    ui->graphicsView->initialize(1, image.rows, image.cols);
    ui->graphicsView->setImage2(image);

    cv::Mat his;
    myCV::histogram(image, his);
    ui->histogram->setPixmap(QPixmap::fromImage(QImage(his.data, his.cols, his.rows, his.step, QImage::Format_RGB888).rgbSwapped()));
}

void MainWindow::on_actionGrayscale_average_triggered()
{
    if(!image.empty())
    {
        cv::Mat show;
        myCV::myCvtColor(image, image, myCV::BGR2GRAY, myCV::GRAY_AVG);
        myCV::myCvtColor(image, show, myCV::GRAY2GBR);
        ui->graphicsView->setImage2(show);

        cv::Mat his;
        myCV::histogram(image, his);
        ui->histogram->setPixmap(QPixmap::fromImage(QImage(his.data, his.cols, his.rows, his.step, QImage::Format_RGB888).rgbSwapped()));
    }
}

void MainWindow::on_actionGrayscale_luminosity_triggered()
{
    if(!image.empty())
    {
        cv::Mat show;
        myCV::myCvtColor(image, image, myCV::BGR2GRAY, myCV::GRAY_LUMI);
        myCV::myCvtColor(image, show, myCV::GRAY2GBR);
        ui->graphicsView->setImage2(show);

        cv::Mat his;
        myCV::histogram(image, his);
        ui->histogram->setPixmap(QPixmap::fromImage(QImage(his.data, his.cols, his.rows, his.step, QImage::Format_RGB888).rgbSwapped()));
    }
}
