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

void subWindow::setImage(cv::Mat &img)
{
    _img = img.clone();
    windowWidget->setImage(_img);
}
