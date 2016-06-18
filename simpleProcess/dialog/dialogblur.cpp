#include "dialogblur.h"
#include "ui_dialogblur.h"

DialogBlur::DialogBlur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBlur)
{
    ui->setupUi(this);
    ui->doubleSpinBox_gaussianSigma->setEnabled(false);
}

DialogBlur::~DialogBlur()
{
    delete ui;
}

int DialogBlur::getMaskSize() const
{
    return this->ui->spinBox_maskSize->value();
}

bool DialogBlur::useGaussian() const
{
    return this->ui->checkBox_useGaussian->isChecked();
}

double DialogBlur::getSigma() const
{
    return this->ui->doubleSpinBox_gaussianSigma->value();
}

void DialogBlur::on_checkBox_useGaussian_stateChanged(int arg1)
{
    if(ui->checkBox_useGaussian->isChecked()){ui->doubleSpinBox_gaussianSigma->setEnabled(true);}
    else{ui->doubleSpinBox_gaussianSigma->setEnabled(false);}
}

void DialogBlur::on_spinBox_maskSize_valueChanged(int arg1)
{
    if(arg1 < 3)
        ui->spinBox_maskSize->setValue(3);
    else if(arg1 > 19)
        ui->spinBox_maskSize->setValue(19);
    else if(arg1 % 2 == 0)
        ui->spinBox_maskSize->setValue(arg1-1);
}
