#include "smartgraphicsview.h"
#include <QStyleOptionGraphicsItem>
#include <QMenu>
#include <QPixmap>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QtConcurrent/QtConcurrentMap>
#include <QTime>
#include <QDebug>
#include <QReadWriteLock>
extern QReadWriteLock lock;

SmartGraphicsView::SmartGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
	this->setTransformationAnchor(QGraphicsView::NoAnchor);
    this->setMouseTracking(true);    
    saveAction = new QAction("Save Image", this);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(on_saveAction_triggered()));
    cam_num = 0;
    scene = new QGraphicsScene;
    this->setScene(scene);
}

SmartGraphicsView::~SmartGraphicsView()
{
    scene->clear();
    delete scene;
}

void SmartGraphicsView::initialize(const int _cam_num, const int width, const int height)
{
    cam_num = _cam_num;
    const size_t CAP_NUM = cam_num;
    //Clear
    scene->clear();
    pix_item_vec.clear();
    //--
    // Add Pixmap Items
    for(size_t i = 0; i < CAP_NUM; ++i){
        QGraphicsPixmapItem *pix_item = scene->addPixmap(QPixmap(width, height));
        pix_item_vec.push_back(pix_item);
    }
//    connect(img_item[0], SIGNAL(sendMouXY(double,double)), this, SIGNAL(sendItemMouXY(double,double)));

    // Layout

    const int spacing = 30;
    if(CAP_NUM == 8){
        pix_item_vec[0]->setPos(0, 0);
        pix_item_vec[4]->setPos(0, height+spacing);
        for(size_t i = 1; i < 4; ++i){
            QPointF p = pix_item_vec[i-1]->pos();
            QPointF p2 = pix_item_vec[i-1+4]->pos();
            pix_item_vec[i]->setPos(p.x()+width+spacing, p.y());
            pix_item_vec[i+4]->setPos(p2.x()+width+spacing, p2.y());
        }
    }
    else{
        pix_item_vec[0]->setPos(0, 0);
        for(size_t i = 1; i < CAP_NUM; ++i){
            QPointF p = pix_item_vec[i-1]->pos();
            pix_item_vec[i]->setPos(p.x()+width+spacing, p.y());
        }
    }
    this->fitInView(0, 0, width*cam_num, height, Qt::KeepAspectRatio);
}

void SmartGraphicsView::setImage(const std::vector<QImage> &imgs)
{
    lock.lockForRead();
    for(size_t i = 0; i < imgs.size(); ++i)
        pix_item_vec[i]->setPixmap(QPixmap::fromImage(imgs[i]));
    lock.unlock();
    QList<QGraphicsItem *> item_list = this->items(this->rect());
    for(int i = 0; i < item_list.size()/2; ++i){
        item_list.at(i)->update();
    }
}

void SmartGraphicsView::on_saveAction_triggered()
{
    if(cam_num == 0) {
        return;
    }
    QString file_name = QFileDialog::getSaveFileName(0, "Img");
    if(file_name.isEmpty()) {
        return;
    }
    for(int i = 0; i < cam_num; ++i)
    {
        //        pix_item_vec[i]->pixmap().save(file_name+QString::number(i)+".jpg");
        pix_item_vec[i]->pixmap().save(file_name+QString::number(i)+".bmp");
    }
}

void SmartGraphicsView::setImage(const std::vector<cv::Mat> &imgs)
{
    lock.lockForRead();
    for(size_t i = 0; i < imgs.size(); ++i){
        QImage img_temp(imgs[i].cols, imgs[i].rows, QImage::Format_RGB888);
        for(int y = 0; y < imgs[i].rows; ++y){
            memcpy(img_temp.scanLine(y), imgs[i].data + y * imgs[i].cols * 3, imgs[i].cols * 3);
        }
        pix_item_vec[i]->setPixmap(QPixmap::fromImage(img_temp.rgbSwapped()));
    }
    lock.unlock();
    QList<QGraphicsItem *> item_list = this->items(this->rect());
    for(int i = 0; i < item_list.size()/2; ++i){
        item_list.at(i)->update();
    }
}


void SmartGraphicsView::setImage2(const cv::Mat &imgs)
{
    QImage img_temp(imgs.cols, imgs.rows, QImage::Format_RGB888);
    lock.lockForRead();
    for(int y = 0; y < imgs.rows; ++y){
        memcpy(img_temp.scanLine(y), imgs.data + y * imgs.cols * 3, imgs.cols * 3);
    }
    lock.unlock();
    pix_item_vec[0]->setPixmap(QPixmap::fromImage(img_temp.rgbSwapped()));
}

void SmartGraphicsView::setImagefromQImage(const QImage &qimg)
{
    pix_item_vec[0]->setPixmap(QPixmap::fromImage(qimg));
}

void SmartGraphicsView::updateImg()
{
    QList<QGraphicsItem *> item_list = this->items(this->rect());
    for(int i = 0; i < item_list.size(); ++i){
        QGraphicsPixmapItem *item = dynamic_cast<QGraphicsPixmapItem*>(item_list[i]);
        if(item)
            item->update();        
    }
}

void SmartGraphicsView::wheelEvent(QWheelEvent *event)
{
	if(event->delta() == 0)
		return;
	QList<QGraphicsItem*> list = this->items();
	if(list.size() <= 0)
		return;

	QPointF pt = this->mapToScene(event->pos());
	double factor;
	if(event->delta() > 0)
		factor = 1.1;
	else if(event->delta() < 0)
		factor = 0.9;
	else
		factor = 1;
	this->scale(factor, factor);
	this->centerOn(pt);
}

void SmartGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::LeftButton){
		this->translate(( -mou_x + event->x())/1.0, ( -mou_y + event->y())/1.0);
		this->setCursor(Qt::ClosedHandCursor);
		//QMessageBox::information(0, QString::number(mou_x - event->x()), QString::number(mou_y - event->y()));
	}
	this->setCursor(Qt::OpenHandCursor);
	mou_x = event->x();
	mou_y = event->y();

}

void SmartGraphicsView::mousePressEvent(QMouseEvent *event)
{
    mou_x = event->x();
	mou_y = event->y();    
	if(event->button() == Qt::LeftButton)
		this->setCursor(Qt::ClosedHandCursor);
    else if(event->button() == Qt::MidButton)
        this->fitInView(0, 0, this->sceneRect().width(), this->sceneRect().height(), Qt::KeepAspectRatio);
    emit sendMousePress();
}

void SmartGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsPixmapItem *item = dynamic_cast<QGraphicsPixmapItem *>(this->itemAt(event->pos()));
    if(!item)
        return;
    QPointF local_pt = item->mapFromScene(this->mapToScene(event->pos()));
    qDebug () << local_pt;
    if(this->pix_item_vec.size() == 1)
        emit sendItemMouXY(local_pt.x(), local_pt.y());
}

void SmartGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
    if(event->button() == Qt::RightButton){
        QMenu m(this);
        m.addAction(saveAction);
        m.exec(event->globalPos());
    }
}
