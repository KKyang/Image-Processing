#ifndef SHAREMEMORY_H
#define SHAREMEMORY_H
#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QImage>
#include <QSharedMemory>
#include "opencv2/opencv.hpp"
enum ImageType{
    RGB = 0,
    GRAY
};

class shareMemory
{
public:
    shareMemory();
    bool addToSharedMemory(cv::Mat &img);
    bool readFromSharedMemory(cv::Mat &img, int imageType = ImageType::RGB);
private:
    QSharedMemory sharedMemory;
};

#endif // SHAREMEMORY_H
