#ifndef DIALOGSIZE_H
#define DIALOGSIZE_H

#include <QDialog>

namespace Ui {
class DialogSize;
}

class DialogSize : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSize(QWidget *parent = 0);
    ~DialogSize();
    int getSize();

private slots:
    void on_spinBox_filterSize_valueChanged(int arg1);

private:
    Ui::DialogSize *ui;
};

#endif // DIALOGSIZE_H
