#ifndef DIALOGSINGLEBAREFFECT_H
#define DIALOGSINGLEBAREFFECT_H

#include <QDialog>
#include "opencv2/opencv.hpp"

#include "imgcore.h"
#include "imageprocessing.h"

namespace Ui {
class DialogSingleBarEffect;
}

class DialogSingleBarEffect : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSingleBarEffect(QWidget *parent = 0);
    ~DialogSingleBarEffect();
    void setDialogName(QString &name);
    void setImg(cv::Mat &img);
    cv::Mat returnImg(){return _imgproc.clone();}
    double returnValue();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_horizontalSlider_sliderReleased();

    void on_doubleSpinBox_editingFinished();

private:
    void setBarInterval(double min, double max, double unit, bool isInt = false);
    void setValue(double val);

    Ui::DialogSingleBarEffect *ui;
    bool _is_int;

    cv::Mat _img, _imgproc;
};

#endif // DIALOGSINGLEBAREFFECT_H
