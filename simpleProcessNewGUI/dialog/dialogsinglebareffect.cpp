#include "dialogsinglebareffect.h"
#include "ui_dialogsinglebareffect.h"

DialogSingleBarEffect::DialogSingleBarEffect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSingleBarEffect)
{
    ui->setupUi(this);
}

DialogSingleBarEffect::~DialogSingleBarEffect()
{
    delete ui;
}

void DialogSingleBarEffect::setDialogName(QString &name)
{
    this->setWindowTitle(name);
    if(!name.compare("Contrast"))
    {
        setBarInterval(-5.0, 5.0, 0.1);
        setValue(0);
    }
    else if(!name.compare("Gray Resolution Scale"))
    {
        setBarInterval(0.0, 8.0, 1);
        setValue(8);
    }
}

void DialogSingleBarEffect::setBarInterval(double min, double max, double unit, bool isInt)
{
    _is_int = isInt;
    ui->horizontalSlider->setMinimum((int)min/unit);
    ui->horizontalSlider->setMaximum((int)max/unit);
    ui->horizontalSlider->setSingleStep(1);
    ui->doubleSpinBox->setMinimum(min);
    ui->doubleSpinBox->setMaximum(max);
    ui->doubleSpinBox->setSingleStep(unit);
    int count = 0;
    double num = unit;
    num = abs(num);
    num = num - int(num);
    while (num != 0)
    {
        num = num * 10;
        count = count + 1;
        num = num - int(num);
    }
    ui->doubleSpinBox->setDecimals(count);
}

void DialogSingleBarEffect::setImg(cv::Mat &img)
{
    _img = img.clone();
    _imgproc = _img.clone();
    ui->view1->initialize(1,_img.cols,_img.rows);

    ui->view1->setImage(_img);
    if(!this->windowTitle().compare("Gray Resolution Scale"))
    {
        myCV::grayResolutionScale(_img, _imgproc, ui->doubleSpinBox->value());
    }
    ui->view2->initialize(1,_imgproc.cols,_imgproc.rows);
    ui->view2->setImage(_imgproc);
}

double DialogSingleBarEffect::returnValue()
{
    return ui->doubleSpinBox->value();
}

void DialogSingleBarEffect::on_horizontalSlider_valueChanged(int value)
{
    ui->doubleSpinBox->setValue(value * ui->doubleSpinBox->singleStep());
}

void DialogSingleBarEffect::on_doubleSpinBox_valueChanged(double arg1)
{
    if(_is_int)
    {
        arg1 = (int) arg1;
        if(arg1 < ui->doubleSpinBox->minimum())
            arg1 = ui->doubleSpinBox->minimum();
        else if(arg1 > ui->doubleSpinBox->maximum())
            arg1 = ui->doubleSpinBox->maximum();

        ui->doubleSpinBox->setValue(arg1);
    }
    ui->horizontalSlider->setValue((int)(arg1/ui->doubleSpinBox->singleStep()));
}

void DialogSingleBarEffect::setValue(double val)
{
    ui->doubleSpinBox->setValue(val);
}

void DialogSingleBarEffect::on_horizontalSlider_sliderReleased()
{
    on_doubleSpinBox_editingFinished();
}

void DialogSingleBarEffect::on_doubleSpinBox_editingFinished()
{
    if(!this->windowTitle().compare("Contrast"))
    {
        std::vector<std::vector<int>> data;
        myCV::histogram(_img, cv::Mat(),data);

        int&& val = myCV::findHistLargestPos(data);
        double&& con = ui->doubleSpinBox->value();
        int&& max_r = 255 - val;
        int&& min_r = val - 0;

        if(con > 0)
        {
            max_r = max_r * (1 - (std::abs(con)/ui->doubleSpinBox->maximum()));
            min_r = min_r * (1 - (std::abs(con)/ui->doubleSpinBox->maximum()));
        }
        if(con < 0)
        {
            max_r = max_r * (1 + (std::abs(con)/ui->doubleSpinBox->maximum()));
            min_r = min_r * (1 + (std::abs(con)/ui->doubleSpinBox->maximum()));
        }
        myCV::myContrast(_img, _imgproc, (val - min_r), (val + max_r));
    }
    else if(!this->windowTitle().compare("Gray Resolution Scale"))
    {
        myCV::grayResolutionScale(_img, _imgproc, ui->doubleSpinBox->value());
    }


    ui->view2->initialize(1,_imgproc.cols, _imgproc.rows);
    ui->view2->setImage(_imgproc);
}
