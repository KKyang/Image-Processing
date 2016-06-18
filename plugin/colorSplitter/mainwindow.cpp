#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowContextHelpButtonHint| Qt::WindowCloseButtonHint);
    cv::Mat bar;

    myCV::getPseudoBar(pseudo_pos, bar, cv::Size(ui->label_pseudoBar->size().width(), ui->label_pseudoBar->size().height()));
    ui->label_pseudoBar->setPixmap(QPixmap::fromImage(QImage(bar.data, bar.cols, bar.rows, bar.step, QImage::Format_RGB888).rgbSwapped()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    sendSig();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setShowImage(cv::Mat &img)
{
    if(colorType == myCV::GRAY && pic_status != 1)
    {
        ui->graphicsView->initialize(1, img.cols, img.rows);
        pic_status = 1;
    }
    else if(colorType != myCV::GRAY && pic_status != 3)
    {
        ui->graphicsView->initialize(4, img.cols, img.rows, 2);
        pic_status = 3;
    }

    std::vector<cv::Mat> img_set(1);

    if(colorType == myCV::GRAY)
    {
        myCV::myCvtColor(img, img, myCV::GRAY2GBR);
        ui->graphicsView->setImage(img);
        return;
    }

    split3(img, colorType, img_set);
    img_set[0] = image.clone();
    ui->graphicsView->setImage(img_set);
}

void MainWindow::setShowImage(std::vector<cv::Mat> &imgs)
{
    if(colorType == myCV::pseudo && pic_status != 2)
    {
        ui->graphicsView->initialize(2, imgs[0].cols, imgs[0].rows);
        pic_status = 2;
    }
    if(colorType == myCV::pseudo)
    {
        myCV::myCvtColor(imgs[0], imgs[0], myCV::GRAY2GBR);
        ui->graphicsView->setImage(imgs);
        return;
    }
}

void MainWindow::guiOnOff()
{

}

void MainWindow::on_actionOpen_image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");

    if(fileName.isEmpty())
        return;
    image = cv::imread(fileName.toStdString());
    if(image.empty())
        return;

    isInitial = false;
    if(image.type() == CV_8UC1)
    {
        colorType = myCV::GRAY;
        ui->radioButton_gray->setChecked(true);
    }
    else if(image.type() == CV_8UC3)
    {
        colorType = myCV::RGB;
        ui->radioButton_rgb->setChecked(true);
    }
    pic_status = 0;
    setShowImage(image);
}

void MainWindow::on_radioButton_hsv_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::myCvtColor(image, temp, myCV::BGR2HSV);
    colorType = myCV::HSV;
    setShowImage(temp);

}

void MainWindow::on_radioButton_rgb_clicked()
{
    if(image.empty())
        return;

    colorType = myCV::RGB;
    setShowImage(image);
}

void MainWindow::on_radioButton_cmy_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::myCvtColor(image, temp, myCV::BGR2CMY);
    colorType = myCV::CMY;
    setShowImage(temp);
}

void MainWindow::on_radioButton_xyz_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::myCvtColor(image, temp, myCV::BGR2XYZ);
    colorType = myCV::XYZ;
    setShowImage(temp);
}

void MainWindow::on_radioButton_cielab_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::myCvtColor(image, temp, myCV::BGR2XYZ);
    myCV::myCvtColor(temp, temp, myCV::XYZ2Lab);
    colorType = myCV::Lab;
    setShowImage(temp);
}

void MainWindow::on_radioButton_yuv_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::myCvtColor(image, temp, myCV::BGR2YUV);
    colorType = myCV::YUV;
    setShowImage(temp);
}

void MainWindow::on_radioButton_gray_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::myCvtColor(image, temp, myCV::BGR2GRAY);
    colorType = myCV::GRAY;
    setShowImage(temp);
}

void MainWindow::on_radioButton_pseudoColor_clicked()
{
    if(image.empty())
        return;

    std::vector<cv::Mat> temp(2);
    myCV::myCvtColor(image, temp[0], myCV::BGR2GRAY);

    myCV::pseudoColor(temp[0], temp[1], pseudo_pos);
    colorType = myCV::pseudo;
    setShowImage(temp);
}

void MainWindow::on_label_pseudoBar_sendHoriDis(int dx)
{
    if(pseudo_pos + dx < 0){pseudo_pos = 359;}
    else if(pseudo_pos + dx >= 360){pseudo_pos = 0;}
    else{pseudo_pos += dx;}

    if(image.empty() || !ui->radioButton_pseudoColor->isChecked())
    {
        cv::Mat bar;
        myCV::getPseudoBar(pseudo_pos, bar, cv::Size(ui->label_pseudoBar->size().width(), ui->label_pseudoBar->size().height()));
        ui->label_pseudoBar->setPixmap(QPixmap::fromImage(QImage(bar.data, bar.cols, bar.rows, bar.step, QImage::Format_RGB888).rgbSwapped()));
        return;
    }
    if(image.cols < 2000 && image.rows < 2000)
    {
        std::vector<cv::Mat> temp(2);
        myCV::myCvtColor(image, temp[0], myCV::BGR2GRAY);

        cv::Mat bar;
        myCV::pseudoColor(temp[0], temp[1], pseudo_pos, true, bar, cv::Size(ui->label_pseudoBar->size().width(), ui->label_pseudoBar->size().height()));
            ui->label_pseudoBar->setPixmap(QPixmap::fromImage(QImage(bar.data, bar.cols, bar.rows, bar.step, QImage::Format_RGB888).rgbSwapped()));
        colorType = myCV::pseudo;
        setShowImage(temp);
    }
    else
    {
        cv::Mat bar;
        myCV::getPseudoBar(pseudo_pos, bar, cv::Size(ui->label_pseudoBar->size().width(), ui->label_pseudoBar->size().height()));
        ui->label_pseudoBar->setPixmap(QPixmap::fromImage(QImage(bar.data, bar.cols, bar.rows, bar.step, QImage::Format_RGB888).rgbSwapped()));
    }
}

void MainWindow::on_label_pseudoBar_released()
{
    if(image.cols > 2000 && image.rows > 2000)
    {
        std::vector<cv::Mat> temp(2);
        myCV::myCvtColor(image, temp[0], myCV::BGR2GRAY);

        cv::Mat bar;
        myCV::pseudoColor(temp[0], temp[1], pseudo_pos, true, bar, cv::Size(ui->label_pseudoBar->size().width(), ui->label_pseudoBar->size().height()));
            ui->label_pseudoBar->setPixmap(QPixmap::fromImage(QImage(bar.data, bar.cols, bar.rows, bar.step, QImage::Format_RGB888).rgbSwapped()));
        colorType = myCV::pseudo;
        setShowImage(temp);
    }
}



void MainWindow::on_actionImport_from_Main_triggered()
{
    image = _from_Main_img.clone();

    if(image.empty())
    {
        QMessageBox::critical(this,"Error","No image is opened in main program.");
        return;
    }

    isInitial = false;
    if(image.type() == CV_8UC1)
    {
        colorType = myCV::GRAY;
        ui->radioButton_gray->setChecked(true);
    }
    else if(image.type() == CV_8UC3)
    {
        colorType = myCV::RGB;
        ui->radioButton_rgb->setChecked(true);
    }
    pic_status = 0;
    setShowImage(image);
}
