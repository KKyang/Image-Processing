/*This is a Qt dialog for user to enter scaling properties.
*/

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

    //Get scaled images original properties.
    void getCurrentImageProperties(unsigned int w, unsigned int h);
    //Check if the user choose to keep aspect ratio.
    bool isAspect();
    //Get scaled-to width.
    int getWidth();
    //Get scaled-to height.
    int getHeight();

private slots:
    void on_lineEdit_resizeW_textEdited(const QString &arg1);
    void on_lineEdit_resizeH_textEdited(const QString &arg1);

private:
    Ui::DialogResize *ui;
    //width, height used to store properties.
    int width, height;
};

#endif // DIALOGRESIZE_H
