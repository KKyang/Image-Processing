#ifndef SHAREMEMORY_H
#define SHAREMEMORY_H
#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QImage>
#include <QSharedMemory>
#include "opencv2/opencv.hpp"
#include "imageprocessing.h"
enum ImageType{
    RGB = 0,
    GRAY
};

class shareMemory
{
public:
    shareMemory();
    bool addToSharedMemory(cv::Mat &img);
    bool readFromSharedMemory(cv::Mat &img, const int imageType = ImageType::RGB);
    void requestDetach();
private:
    QSharedMemory sharedMemory;
};

#endif // SHAREMEMORY_H
