#ifndef PTZLABEL_H
#define PTZLABEL_H

#include <QLabel>
#include <QImage>
#include <QDebug>
#include <QDateTime>


class PTZLabel : public QLabel
{
    Q_OBJECT
public:
    explicit PTZLabel(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
signals:
    void sendMouXY(double x, double y); // 0~1, 0~1
public slots:
private slots:
    void on_saveAction_triggered();
private:

    QAction *saveAction;
};

#endif // PTZLABEL_H
