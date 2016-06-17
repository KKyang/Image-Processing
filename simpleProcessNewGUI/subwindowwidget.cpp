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
    windowWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWidget(windowWidget);
}

subWindow::~subWindow()
{
    windowWidget->~subWindowWidget();
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

void subWindow::setImage(cv::Mat &img)
{
    _img = img.clone();
    windowWidget->setImage(_img);
}
