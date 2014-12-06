#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_client = new LocalSocketIpcClient("simpleProcessing", this);
    m_server = new LocalSocketIpcServer("colorSplitTool", this);

    connect(m_server, SIGNAL(messageReceived(QString)), this, SLOT(socketIcpMessage(QString)));
    connect(m_client, SIGNAL(socketClientStatus(int)), this, SLOT(socketClientStatus(int)));
    mem = new shareMemory();


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
    if(message == "requestImage")
    {
        if(image.empty())
        {
            m_client->sendMessageToServer("No image.");
            return;
        }
        if(mem->addToSharedMemory(image))
            m_client->sendMessageToServer("ok "+QString::number(image.type()));
        else
            m_client->sendMessageToServer("Porting fail.");
    }
    else if(message.contains("ok "))
    {
        int imageType = message.right(3).toInt();
        if(mem->readFromSharedMemory(image, imageType))
        {
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

void MainWindow::setShowImage(cv::Mat &img)
{
    if(!isInitial)
    {
        if(colorType == myCV::GRAY)
            ui->graphicsView->initialize(1, img.cols, img.rows);
        else
            ui->graphicsView->initialize(4, img.cols, img.rows, 2);
        isInitial = true;
    }
    std::vector<cv::Mat> img_set(1);
    if(img.type() == CV_8UC1)
    {
        cv::Mat tmp;
        myCV::myCvtColor(img, tmp, myCV::GRAY2GBR);
        img_set[0] = tmp.clone();
        ui->graphicsView->setImage(img_set);
        return;
    }

    split3(img, colorType, img_set);
    img_set[0] = image.clone();
    ui->graphicsView->setImage(img_set);
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
