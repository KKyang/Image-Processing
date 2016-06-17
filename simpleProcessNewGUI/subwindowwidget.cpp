#include "subwindowwidget.h"
#include "ui_subwindowwidget.h"

subWindowWidget::subWindowWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subWindowWidget)
{
    ui->setupUi(this);
}

subWindowWidget::~subWindowWidget()
{
    delete ui;
}

void subWindowWidget::setImage(cv::Mat &img)
{
    ui->widget->initialize(1,img.cols,img.rows);
    ui->widget->setImage(img);
}

void subWindowWidget::setImage(QImage &img)
{
    ui->widget->initialize(1,img.width(),img.height());
    ui->widget->setImagefromQImage(img);
}

subWindow::subWindow(QWidget *parent) :
  QMdiSubWindow(parent)
{
    windowWidget = new subWindowWidget(this);
    this->setWidget(windowWidget);
}

subWindow::~subWindow()
{
    windowWidget->~subWindowWidget();
}

void subWindow::backupImage()
{
    forwardImg.clear();
    //ui->actionForward->setEnabled(false);
    if(recoverImg.size() > recoverLimit)
    {
        recoverImg.erase(recoverImg.begin());
    }
    recoverImg.push_back(_img);
}

void subWindow::refreshImg()
{
    if(_img.type()==CV_8UC3)
    {
       windowWidget->setImage(_img);
    }
    else if(_img.type() == CV_8UC1)
    {
        cv::Mat show;
        myCV::myCvtColor(_img, show, myCV::GRAY2GBR);
        windowWidget->setImage(show);
    }
}

/*
 * void MainWindow::on_actionBack_triggered() //undo function
{
    forwardImg.push_back(image);
    image = recoverImg[recoverImg.size()-1].clone();
    recoverImg.erase(recoverImg.end()-1);
    setShowImage(image);

    if(recoverImg.empty())
        ui->actionBack->setEnabled(false);
    ui->actionForward->setEnabled(true);
    ui->statusBar->showMessage("Undo");
}

void MainWindow::on_actionForward_triggered()  //redo function
{
    recoverImg.push_back(image);
    image = forwardImg[forwardImg.size()-1].clone();
    forwardImg.erase(forwardImg.end()-1);
    setShowImage(image);

    if(forwardImg.empty())
        ui->actionForward->setEnabled(false);
    ui->actionBack->setEnabled(true);
    ui->statusBar->showMessage("Redo");
}
*/

void subWindow::setImage(cv::Mat &img)
{
    _img = img.clone();
    windowWidget->setImage(_img);
}
