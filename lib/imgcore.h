#ifndef IMGCORE_H
#define IMGCORE_H
#include "opencv2/opencv.hpp"

namespace myCV{
enum{
    NEAREST = 1,
    BILINEAR
};

enum normType{
    NORM_INF = 0,
    NORM_L1,
    NORM_L2,
    MINMAX
};
template<typename T>
void findMinMax(const cv::Mat &img, T &min, T &max);

//Resize image.
void myResize(cv::Mat &inputArray, cv::Mat &outputArray, const unsigned int _width, unsigned int _height,
              const bool _aspectRatioFix = 1, const int method = 2);

void normalize(cv::Mat &inputArray, cv::Mat &outputArray, int min, int max, int type = normType::NORM_L2);

class imgCore
{

public:
    imgCore();


private:

};
}

//Might be wrong, need to check.
template<typename T>
void myCV::findMinMax(const cv::Mat &img, T &min, T &max)
{
    min = img.ptr<T>(0)[0]; max = img.ptr<T>(0)[0];
    for (int j = 0; j < img.rows; j++)
    {
        for (int i = 0; i < img.cols * img.channels(); i++)
        {
            min = img.ptr<T>(j)[i] < min ? img.ptr<T>(j)[i] : min;
            max = img.ptr<T>(j)[i] > max ? img.ptr<T>(j)[i] : max;
        }
    }
}

#endif // IMGCORE_H
