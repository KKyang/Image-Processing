#include "qlabelbar.h"
qLabelBar::qLabelBar(QWidget *parent) :
    QLabel(parent)
{
}

void qLabelBar::mousePressEvent(QMouseEvent *ev)
{
    first = ev->pos();
}

void qLabelBar::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->buttons() == Qt::LeftButton)
    {
        emit sendHoriDis((int)(ev->pos().x() - first.x()));
        first = ev->pos();
    }
}

void qLabelBar::mouseReleaseEvent(QMouseEvent *ev)
{
    emit released();
}
