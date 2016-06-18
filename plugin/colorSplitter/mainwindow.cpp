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

void MainWindow::closeEvent(QCloseEvent *event)
{
    sendSig();
}

MainWindow::~MainWindow()
{
    delete ui;
}
