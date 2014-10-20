/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                      (The BSD 3-Clause License)
//
// Copyright (C) 2014, Shih-Jhong Yu, KKyang, NTU BIME BBLab
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef SMARTGRAPHICSVIEW_H
#define SMARTGRAPHICSVIEW_H

#include <QFileInfo>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QList>
#include <QWheelEvent>
#include <QMessageBox>
#include <QVector>

#include <vector>
#include "opencv2/opencv.hpp"

class SmartGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
	explicit SmartGraphicsView(QWidget *parent = 0);
    ~SmartGraphicsView();
    void initialize(const int _img_num, const int width, const int height, int changeRow = 4);
    void setImage(const cv::Mat &img);
    void setImage(const std::vector<cv::Mat> &imgs);
    void setImagefromQImage(const QImage &qimg);
    void setImagefromQImage(const std::vector<QImage> &qimgs);
    int getImgNum(){return img_num;}
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
    void updateImg();
private slots:
    void on_saveAction_triggered();
private:
    QAction *saveAction;
    QGraphicsScene *scene;
    QVector<QGraphicsPixmapItem*> pix_item_vec;
	int mou_x;
	int mou_y;
    int img_num;
};


#endif // SMARTGRAPHICSVIEW_H
