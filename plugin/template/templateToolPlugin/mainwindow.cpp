#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowContextHelpButtonHint| Qt::WindowCloseButtonHint);
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

void MainWindow::receiveRequestedImage()
{
    if( _from_Main_img.empty())
    {
        QMessageBox::critical(this,"Error","No image is opened in main program.");
        return;
    }
    image = _from_Main_img.clone();
    isImageImported = true;
}
