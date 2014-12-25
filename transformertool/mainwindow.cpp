#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_client = new LocalSocketIpcClient("simpleProcessing", this);
    m_server = new LocalSocketIpcServer("transformerTool", this);

    connect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
    connect(m_client, SIGNAL(socketClientStatus(int)), this, SLOT(socketClientStatus(int)));
    mem = new shareMemory();

    ui->horizontalSlider_lineThreshold->setValue(70);
    ui->spinBox_lineThresholdValue->setValue(70);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    disconnect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
    disconnect(m_client, SIGNAL(socketClientStatus(int)), this, SLOT(socketClientStatus(int)));
    if(mem){delete mem;}
    if(m_client){m_client->deleteLater();}
    if(m_server){m_server->deleteLater();}
}

void MainWindow::socketClientStatus(int status)
{
    // 0 - Ready, 1 - disconnected, 2 - error
    if(status == 2)
    {
        QMessageBox::warning(0, "Error", "Simple Processing Application is not opened.");
    }
}

void MainWindow::socketIcpMessage(QString message)
{
    //qDebug()<< message;
    if(message == "requestImage")
    {
        if(image.empty())
        {
            m_client->sendMessageToServer("2 No image.");
            return;
        }
        if(mem->addToSharedMemory(image))
            m_client->sendMessageToServer("2 ok "+QString::number(image.type()));
        else
            m_client->sendMessageToServer("2 Porting fail.");
    }
    else if(message.contains("ok "))
    {
        int imageType = message.right(3).toInt();
        if(mem->readFromSharedMemory(image, imageType))
        {
            /* Where your code is */
            setShowImage(image);

            m_client->sendMessageToServer("2 done");
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

void MainWindow::setShowImage(cv::Mat &img)
{
    ui->graphicsView->setImage(img);
}

void MainWindow::setShowImage(std::vector<cv::Mat> &imgs)
{
    if(image.empty())
    {
        QMessageBox::warning(0, "Error", "No image is opened!");
        return;
    }
    m_client->sendMessageToServer("2 ImportImage");
}

void MainWindow::on_actionOpen_image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),0,"Image files (*.png *.bmp *.jpg);;PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");

    if(fileName.isEmpty())
        return;
    image = cv::imread(fileName.toStdString());
    if(image.empty())
        return;

    ui->graphicsView->initialize(1, image.cols, image.rows);
    setShowImage(image);
}

void MainWindow::on_actionImport_photo_from_Simple_Processing_triggered()
{
    m_client->sendMessageToServer("2 requestImage");
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

void MainWindow::on_actionExport_Image_to_Simple_Processing_triggered()
{
    m_client->sendMessageToServer("2 requestImage");
}
