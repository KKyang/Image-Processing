#include "dialogresize.h"
#include "ui_dialogresize.h"

DialogResize::DialogResize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogResize)
{
    ui->setupUi(this);
    ui->lineEdit_resizeW->setValidator(new QIntValidator(1,10000,this));
    ui->lineEdit_resizeH->setValidator(new QIntValidator(1,10000,this));
}

DialogResize::~DialogResize()
{
    delete ui;
}

void DialogResize::getCurrentImageProperties(unsigned int w, unsigned int h)
{
    ui->lineEdit_resizeW->setText(QString::number(w));
    ui->lineEdit_resizeH->setText(QString::number(h));
    width = w;
    height = h;
}

bool DialogResize::isAspect()
{
    return ui->checkBox_keepAspect->isChecked();
}

int DialogResize::getWidth()
{
    return ui->lineEdit_resizeW->text().toInt();
}

int DialogResize::getHeight()
{
    return ui->lineEdit_resizeH->text().toInt();
}

void DialogResize::on_lineEdit_resizeW_textEdited(const QString &arg1)
{
    if(ui->checkBox_keepAspect->isChecked())
    {
        int newH = (int)(height*((double)arg1.toInt() / (double)width));
        ui->lineEdit_resizeH->setText(QString::number(newH));
    }
}

void DialogResize::on_lineEdit_resizeH_textEdited(const QString &arg1)
{
    if(ui->checkBox_keepAspect->isChecked())
    {
        int newW = (int)(width*((double)arg1.toInt() / (double)height));
        ui->lineEdit_resizeW->setText(QString::number(newW));
    }
}
