#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowContextHelpButtonHint| Qt::WindowCloseButtonHint);

    ui->horizontalSlider_lineThreshold->setValue(70);
    ui->spinBox_lineThresholdValue->setValue(70);

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
    delete ui;
}

void MainWindow::uiEnabler(bool c)
{
    //ui control
    //ui->actionSave_image->setEnabled(c);
    ui->actionExport_to_Main->setEnabled(c);
}

void MainWindow::receiveRequestedImage()
{
    qDebug() << "funnn";

    if(_from_Main_img.empty())
    {
        QMessageBox::critical(this,"Error","No image is opened in main program.");
        return;
    }

    image = _from_Main_img.clone();
    qDebug() << "fun";
    isImageImported = true;
    ui->graphicsView->initialize(1, image.cols, image.rows);
    setShowImage(image);
}

void MainWindow::setShowImage(cv::Mat &img)
{
    ui->graphicsView->setImage(img);
    uiEnabler(true);
}

void MainWindow::setShowImage(std::vector<cv::Mat> &imgs)
{
    if(image.empty())
    {
        QMessageBox::warning(0, "Error", "No image is opened!");
        return;
    }
    ui->graphicsView->setImage(imgs);
}

void MainWindow::on_actionOpen_image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");

    if(fileName.isEmpty())
        return;
    cv::Mat tmp;
    tmp = cv::imread(fileName.toStdString());
    if(tmp.empty())
        return;

    tmp = image.clone();
    ui->graphicsView->initialize(1, image.cols, image.rows);
    setShowImage(image);
}

void MainWindow::on_pushButton_tilt_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::tilt(image, temp);
    setShowImage(temp);
}

void MainWindow::on_pushButton_wave_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::wave(image, temp);
    setShowImage(temp);
}

void MainWindow::on_pushButton_original_clicked()
{
    if(image.empty())
        return;

    setShowImage(image);
}

void MainWindow::on_pushButton_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::fisheye(image, temp);
    setShowImage(temp);
}

void MainWindow::on_pushButton_houghLine_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::HoughLineDetection(image, temp, ui->horizontalSlider_lineThreshold->value());
    setShowImage(temp);
}

void MainWindow::on_horizontalSlider_lineThreshold_sliderMoved(int position)
{
    ui->spinBox_lineThresholdValue->setValue(position);
}

void MainWindow::on_spinBox_lineThresholdValue_editingFinished()
{
    ui->horizontalSlider_lineThreshold->setValue(ui->spinBox_lineThresholdValue->value());
}

void MainWindow::on_pushButton_houghRectangle_clicked()
{
    if(image.empty())
        return;

    cv::Mat temp;
    myCV::HoughRectangleDetection(image, temp, ui->horizontalSlider_lineThreshold->value());
    setShowImage(temp);
}

void MainWindow::on_actionImport_from_Main_triggered()
{
    sendImageRequest();
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
