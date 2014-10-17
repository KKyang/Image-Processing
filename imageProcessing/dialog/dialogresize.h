#ifndef DIALOGRESIZE_H
#define DIALOGRESIZE_H

#include <QDialog>

namespace Ui {
class DialogResize;
}

class DialogResize : public QDialog
{
    Q_OBJECT

public:
    explicit DialogResize(QWidget *parent = 0);
    ~DialogResize();
    void getCurrentImageProperties(unsigned int w, unsigned int h);

    bool isAspect();
    int getWidth();
    int getHeight();

private slots:
    void on_lineEdit_resizeW_textEdited(const QString &arg1);
    void on_lineEdit_resizeH_textEdited(const QString &arg1);

private:
    Ui::DialogResize *ui;
    int width, height;
};

#endif // DIALOGRESIZE_H
