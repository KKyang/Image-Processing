#ifndef DIALOGBLUR_H
#define DIALOGBLUR_H

#include <QDialog>

namespace Ui {
class DialogBlur;
}

class DialogBlur : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBlur(QWidget *parent = 0);
    ~DialogBlur();

    int getMaskSize() const;
    bool useGaussian() const;
    double getSigma() const;

private slots:
    void on_checkBox_useGaussian_stateChanged(int arg1);

private:
    Ui::DialogBlur *ui;
};

#endif // DIALOGBLUR_H
