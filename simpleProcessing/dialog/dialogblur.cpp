#include "dialogblur.h"
#include "ui_dialogblur.h"

DialogBlur::DialogBlur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBlur)
{
    ui->setupUi(this);
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
    if(ui->checkBox_useGaussian->isChecked()){ui->doubleSpinBox_gaussianSigma->setEnabled(false);}
    else{ui->doubleSpinBox_gaussianSigma->setEnabled(true);}
}
