#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H
#include "opencv2/opencv.hpp"

namespace myCV{

enum{
    BGR2GRAY = 1,
    GRAY2GBR
};
enum{
    GRAY_AVG = 1,
    GRAY_LUMI
};

void myCvtColor(cv::Mat &inputArray, cv::Mat &outputArray, int colorType, int grayType=2);

void myThreshold(cv::Mat &inputArray, cv::Mat &outputArray, unsigned int _threshold, unsigned _min, unsigned _max);

void histogram(cv::Mat &inputArray, cv::Mat &histogram);

class imageProcessing
{

public:
    imageProcessing();


};



}



#endif // IMAGEPROCESSING_H
