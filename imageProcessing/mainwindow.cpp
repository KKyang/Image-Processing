#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionBack->setEnabled(false);
    ui->actionForward->setEnabled(false);

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::setShowImage(cv::Mat &img)
{
    cv::Mat his;
    myCV::histogram(image, his);
    ui->histogram->setPixmap(QPixmap::fromImage(QImage(his.data, his.cols, his.rows, his.step, QImage::Format_RGB888).rgbSwapped()));

    if(img.type()==CV_8UC3)
    {
       ui->graphicsView->setImage2(image);
    }
    else if(img.type() == CV_8UC1)
    {
        cv::Mat show;
        myCV::myCvtColor(image, show, myCV::GRAY2GBR);
        ui->graphicsView->setImage2(show);
    }
}

void MainWindow::backupImage(cv::Mat &img)
{
    forwardImg.clear();
    ui->actionForward->setEnabled(false);
    if(recoverImg.size() > recoverLimit)
    {
        recoverImg.erase(recoverImg.begin());
    }
    recoverImg.push_back(img);
}

void MainWindow::on_actionOpen_Image_triggered()
{
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"));
    image = cv::imread(fileName.toStdString());
    ui->graphicsView->initialize(1, image.rows, image.cols);
    setShowImage(image);
}

void MainWindow::on_actionGrayscale_average_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::myCvtColor(image, image, myCV::BGR2GRAY, myCV::GRAY_AVG);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
    }
}

void MainWindow::on_actionGrayscale_luminosity_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        myCV::myCvtColor(image, image, myCV::BGR2GRAY, myCV::GRAY_LUMI);
        setShowImage(image);
        ui->actionBack->setEnabled(true);
    }
}

void MainWindow::on_actionResize_triggered()
{
    if(!image.empty())
    {
        backupImage(image);
        DialogResize r;
        r.getCurrentImageProperties(image.size().width, image.size().height);
        r.exec();
        myCV::myResize(image, image,r.getWidth(),r.getHeight(),r.isAspect());
        setShowImage(image);
        ui->actionBack->setEnabled(true);
    }
}

void MainWindow::on_actionBack_triggered()
{
    forwardImg.push_back(image);
    image = recoverImg[recoverImg.size()-1].clone();
    recoverImg.erase(recoverImg.end()-1);
    setShowImage(image);

    if(recoverImg.empty())
        ui->actionBack->setEnabled(false);
    ui->actionForward->setEnabled(true);
}

void MainWindow::on_actionForward_triggered()
{
    recoverImg.push_back(image);
    image = forwardImg[forwardImg.size()-1].clone();
    forwardImg.erase(forwardImg.end()-1);
    setShowImage(image);

    if(forwardImg.empty())
        ui->actionForward->setEnabled(false);
    ui->actionBack->setEnabled(true);
}
