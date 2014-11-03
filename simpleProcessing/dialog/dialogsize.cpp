#include "dialogsize.h"
#include "ui_dialogsize.h"

DialogSize::DialogSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSize)
{
    ui->setupUi(this);
}

DialogSize::~DialogSize()
{
    delete ui;
}

int DialogSize::getSize()
{
    return ui->spinBox_filterSize->value();
}

void DialogSize::on_spinBox_filterSize_valueChanged(int arg1)
{
    if(arg1 < 3)
    {
        ui->spinBox_filterSize->setValue(3);
    }
    else if(arg1>20)
    {
        ui->spinBox_filterSize->setValue(19);
    }
    else if(arg1%2==0)
    {
        ui->spinBox_filterSize->setValue(arg1-1);
    }

}
