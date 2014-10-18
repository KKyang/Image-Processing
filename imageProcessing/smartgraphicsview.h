#ifndef SMARTGRAPHICSVIEW_H
#define SMARTGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMessageBox>
#include <QList>
#include <QVector>
#include <vector>
#include "opencv2/opencv.hpp"

class SmartGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
	explicit SmartGraphicsView(QWidget *parent = 0);
    ~SmartGraphicsView();
    void initialize(const int cam_num, const int width, const int height);
    void setImage(const std::vector<cv::Mat> &imgs);
    void setImage2(const cv::Mat &imgs);
    void setImagefromQImage(const QImage &qimg);
    int getCamNum(){return cam_num;}
protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void sendItemMouXY(const double x, const double y);
    void sendMousePress();
public slots:
    void setImage(const std::vector<QImage> &imgs);
    void updateImg();
private slots:
    void on_saveAction_triggered();
private:
    QAction *saveAction;
    QGraphicsScene *scene;
    QVector<QGraphicsPixmapItem*> pix_item_vec;
	int mou_x;
	int mou_y;
    int cam_num;
};


#endif // SMARTGRAPHICSVIEW_H
